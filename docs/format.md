# JSON Format Specification

## Overview

The Anki app uses a streaming-friendly format to handle large decks without exhausting ESP32-C3 RAM (400KB total). Traditional JSON arrays require loading the entire deck into memory, which would crash the device with decks larger than ~500 cards.

**Solution**: Use **JSONL (Line-Delimited JSON)** format that allows streaming one card at a time.

## Deck Format

Each deck consists of two files:

1. `deck-metadata.json` - Small metadata file (fits in RAM)
2. `cards.jsonl` - Line-delimited JSON for cards (streamed from SD)

### deck-metadata.json

```json
{
  "id": "spanish-101",
  "name": "Spanish 101",
  "description": "Basic Spanish vocabulary",
  "cardCount": 150,
  "created": "2026-02-08",
  "tags": ["spanish", "vocabulary", "beginner"]
}
```

**Fields:**

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| id | string | Yes | Unique identifier for the deck (folder-safe) |
| name | string | Yes | Display name shown in deck list |
| description | string | No | Brief description |
| cardCount | number | Yes | Total number of cards |
| created | string | No | ISO 8601 date (YYYY-MM-DD) |
| tags | array | No | Array of string tags |

### cards.jsonl

Line-delimited JSON format. Each line is a valid JSON object representing one card:

```jsonl
{"id": "1", "front": "Hello", "back": "Hola", "tags": ["greetings"]}
{"id": "2", "front": "Goodbye", "back": "Adiós", "tags": ["greetings"]}
{"id": "3", "front": "Thank you", "back": "Gracias", "tags": ["courtesy"]}
{"id": "4", "front": "Please", "back": "Por favor", "tags": ["courtesy"]}
```

**Card Fields:**

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| id | string | Yes | Unique card identifier within deck |
| front | string | Yes | HTML content for card front |
| back | string | Yes | HTML content for card back |
| tags | array | No | Array of string tags for categorization |

**Notes:**
- Each line must be valid JSON
- No trailing commas
- UTF-8 encoding
- Front/back content supports basic HTML (see HTML Support section)

## Progress Format

### progress.json

```json
{
  "deckId": "spanish-101",
  "lastReview": "2026-02-08",
  "cards": {
    "1": {
      "ease": 2.5,
      "interval": 6,
      "repetitions": 3,
      "due": "2026-02-14"
    },
    "2": {
      "ease": 2.3,
      "interval": 1,
      "repetitions": 0,
      "due": "2026-02-09"
    }
  }
}
```

**Fields:**

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| deckId | string | Yes | Links to deck metadata |
| lastReview | string | No | Last review session date (YYYY-MM-DD) |
| cards | object | Yes | Map of cardId to progress data |

**Card Progress Fields:**

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| ease | number | Yes | Ease factor (1.3 to ∞, typically 1.3-3.0) |
| interval | number | Yes | Days until next review |
| repetitions | number | Yes | Consecutive correct reviews count |
| due | string | Yes | Next review date (YYYY-MM-DD) |

## HTML Support

Card content (front and back) supports a limited subset of HTML:

### Allowed Tags

| Tag | Purpose |
|-----|---------|
| `<b>` or `<strong>` | Bold text |
| `<i>` or `<em>` | Italic text |
| `<br>` | Line break |
| `<p>` | Paragraph |
| `<div>` | Division/block |
| `<ul>` | Unordered list |
| `<li>` | List item |

### Examples

**Bold text:**
```html
<b>Important</b> word
```

**Italic text:**
```html
<i>emphasis</i>
```

**Line breaks:**
```html
Line one<br>Line two
```

**Lists:**
```html
<ul>
  <li>Item one</li>
  <li>Item two</li>
</ul>
```

### Ignored Content

- HTML attributes (e.g., `style="color: red"`) are stripped
- Unsupported tags are stripped (content preserved)
- CSS classes are ignored
- JavaScript is not executed

## Size Limits

**Maximum upload size: 10MB**

This limit prevents:
- RAM exhaustion during upload
- Excessive flash wear from large writes
- Timeout issues with slow SD cards

**Recommendations:**
- Split large decks (>1000 cards) into smaller topical decks
- Typical deck size: 100-500 cards
- Average card size: ~200 bytes (text only)

## Examples

### Complete Example Deck

**deck-metadata.json:**
```json
{
  "id": "spanish-basics",
  "name": "Spanish Basics",
  "description": "Essential Spanish vocabulary for beginners",
  "cardCount": 3,
  "created": "2026-02-08",
  "tags": ["spanish", "beginner"]
}
```

**cards.jsonl:**
```jsonl
{"id": "1", "front": "<b>Hello</b>", "back": "<b>Hola</b>", "tags": ["greetings"]}
{"id": "2", "front": "Goodbye", "back": "Adiós", "tags": ["greetings"]}
{"id": "3", "front": "Thank you<br><i>very much</i>", "back": "Gracias", "tags": ["courtesy"]}
```

**progress.json (after some reviews):**
```json
{
  "deckId": "spanish-basics",
  "lastReview": "2026-02-08",
  "cards": {
    "1": {
      "ease": 2.5,
      "interval": 6,
      "repetitions": 3,
      "due": "2026-02-14"
    },
    "2": {
      "ease": 2.5,
      "interval": 1,
      "repetitions": 1,
      "due": "2026-02-09"
    },
    "3": {
      "ease": 1.8,
      "interval": 0,
      "repetitions": 0,
      "due": "2026-02-08"
    }
  }
}
```

## Rationale for JSONL

### Traditional JSON Approach (Problematic)

```json
{
  "id": "spanish-101",
  "name": "Spanish 101",
  "cards": [
    {"id": "1", "front": "Hello", "back": "Hola"},
    {"id": "2", "front": "Goodbye", "back": "Adiós"},
    ...
    {"id": "1000", "front": "...", "back": "..."}
  ]
}
```

**Problems:**
- Must parse entire file to access any card
- All cards loaded into RAM simultaneously
- 1000 cards × 200 bytes = 200KB just for card data
- Plus JSON parsing overhead = ~400KB+ RAM usage
- **Result**: Crash on ESP32-C3 with 400KB total RAM

### JSONL Approach (Streaming)

```jsonl
{"id": "1", "front": "Hello", "back": "Hola"}
{"id": "2", "front": "Goodbye", "back": "Adiós"}
...
{"id": "1000", "front": "...", "back": "..."}
```

**Advantages:**
- Read one line at a time from SD card
- Parse one card at a time
- Process and immediately discard
- Memory usage: ~1KB constant (one card buffer)
- Deck size limited only by SD card capacity, not RAM
- **Result**: Works reliably on ESP32-C3

### Implementation Pattern

```cpp
// Open file for streaming
File cardFile = SD.open("/decks/spanish/cards.jsonl");

// Read cards one at a time
while (cardFile.available()) {
  String line = cardFile.readStringUntil('\n');
  Card card = parseCard(line);
  
  // Process card (display, update, etc.)
  processCard(card);
  
  // Card goes out of scope, memory freed
}
```

This streaming approach is essential for handling large Anki decks on resource-constrained embedded devices.
