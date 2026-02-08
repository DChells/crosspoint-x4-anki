// apkg-parser.js

// Load dependencies from local paths
if (typeof importScripts === 'function') {
    importScripts('../lib/jszip.min.js');
    importScripts('../lib/sql-wasm.js');
}

class ApkgParser {
    constructor() {
        this.SQL = null;
        this.initPromise = this.initSqlJs();
    }
    
    async initSqlJs() {
        // sql.js initialization
        const pathPrefix = (typeof importScripts === 'function') ? '../lib/' : 'lib/';

        this.SQL = await initSqlJs({
            locateFile: file => `${pathPrefix}${file}`
        });
    }
    
    async parseApkg(file, onProgress) {
        await this.initPromise;
        
        // 1. Read file as ArrayBuffer
        const arrayBuffer = await file.arrayBuffer();
        
        // 2. Extract ZIP
        const zip = await JSZip.loadAsync(arrayBuffer);
        
        // 3. Find SQLite database
        const dbFile = zip.file('collection.anki21') || zip.file('collection.anki2');
        if (!dbFile) {
            throw new Error('No Anki database found in .apkg');
        }
        
        // 4. Extract database
        const dbData = await dbFile.async('uint8array');
        
        // 5. Open with sql.js
        const db = new this.SQL.Database(dbData);
        
        // 6. Query notes table
        const notesResult = db.exec(`
            SELECT id, flds, tags FROM notes
        `);
        
        // 7. Query cards table (for card data)
        const cardsResult = db.exec(`
            SELECT id, nid, ord, type, queue, due, ivl, factor, reps, lapses 
            FROM cards
        `);
        
        // 8. Build deck structure
        const deck = {
            id: this.generateDeckId(file.name),
            name: file.name.replace('.apkg', ''),
            cardCount: 0,
            cards: []
        };
        
        // 9. Parse notes (fields split by 0x1f)
        if (notesResult.length > 0) {
            const columns = notesResult[0].columns;
            const values = notesResult[0].values;
            
            for (let i = 0; i < values.length; i++) {
                const row = values[i];
                const id = row[0];
                const flds = row[1];
                const tags = row[2];
                
                // Split fields by 0x1f (unit separator)
                const fields = flds.split('\x1f');
                
                deck.cards.push({
                    id: id.toString(),
                    front: fields[0] || '',
                    back: fields[1] || '',
                    tags: tags ? tags.split(' ') : []
                });
                
                if (onProgress) {
                    onProgress(i + 1, values.length);
                }
            }
        }
        
        deck.cardCount = deck.cards.length;
        
        // 10. Close database
        db.close();
        
        return deck;
    }
    
    generateDeckId(filename) {
        return filename
            .toLowerCase()
            .replace('.apkg', '')
            .replace(/[^a-z0-9]/g, '-');
    }
}

// Export for use in upload.html
if (typeof module !== 'undefined' && module.exports) {
    module.exports = ApkgParser;
}
