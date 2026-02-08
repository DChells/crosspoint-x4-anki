# Anki X4 Testing Guide

This document provides comprehensive testing instructions for the Anki flashcard app on CrossPoint X4.

---

## Prerequisites

- CrossPoint X4 device with firmware 0.16.0+ (PR #679 app extension support)
- WiFi network accessible from both device and computer
- Sample Anki deck (.apkg file) - see [Test Decks](#test-decks) below
- Battery > 20% (required for app installation)

---

## Test Decks

### Option A: Download existing decks
- https://ankiweb.net/shared/decks - Browse community decks
- Recommended small decks for testing:
  - "Countries and Capitals" (~200 cards)
  - "Basic English Vocabulary" (~100 cards)

### Option B: Create a minimal test deck
1. Install Anki desktop: https://apps.ankiweb.net/
2. Create a new deck called "Test"
3. Add 5-10 simple cards (front: question, back: answer)
4. File → Export → select "Test" deck → Export as .apkg

### Known Limitations (v0.1.0)
- **Max upload size**: ~10MB
- **Text only**: No images or audio (stripped during conversion)
- **English/Latin-1**: Unicode characters may not render correctly
- **No cloze deletions**: Basic cards only

---

## Installation Test

### 1. Download the Release

```
https://github.com/DChells/crosspoint-x4-anki/releases/tag/v0.1.0
```

Download: `anki-v0.1.0.zip`

### 2. Upload via CrossPoint File Transfer

| Step | Action | Expected Result |
|------|--------|-----------------|
| 2.1 | On X4: Home → File Transfer | File Transfer screen appears |
| 2.2 | Connect to WiFi (STA) or create hotspot (AP) | IP address displayed on screen |
| 2.3 | On computer: open the URL shown | CrossPoint web interface loads |
| 2.4 | Click **Apps** tab | Apps management page opens |
| 2.5 | Upload `anki-v0.1.0.zip` | Upload progress shown, success message |

**Verify**: App appears in the list with name "Anki X4"

### 3. Install the App

| Step | Action | Expected Result |
|------|--------|-----------------|
| 3.1 | On X4: Home → Apps | Apps list shows "Anki X4" |
| 3.2 | Select "Anki X4" | App details screen |
| 3.3 | Press Install | Progress indicator, device reboots |
| 3.4 | Wait for reboot | Anki app main menu appears |

**Checkpoint**: If installation fails, note:
- Battery level
- Error message (if any)
- Device behavior

---

## App Functionality Tests

### Test 1: Main Menu Navigation

| Step | Action | Expected Result |
|------|--------|-----------------|
| 1.1 | App boots | Main menu with options visible |
| 1.2 | Observe menu items | Should show: "Study", "Upload Deck", "Return to CrossPoint" |
| 1.3 | Navigate with buttons | Highlight moves between options |

**Report**:
- [ ] PASS - Menu displays correctly
- [ ] FAIL - Describe issue: _______________

### Test 2: Deck Upload (WiFi)

| Step | Action | Expected Result |
|------|--------|-----------------|
| 2.1 | Select "Upload Deck" | WiFi setup screen or upload instructions |
| 2.2 | Connect to WiFi | IP address displayed |
| 2.3 | On computer: open IP in browser | Upload page loads |
| 2.4 | Upload .apkg file | Progress bar, conversion starts |
| 2.5 | Wait for "Success" | Deck added confirmation |
| 2.6 | Return to main menu | New deck visible in deck list |

**Report**:
- [ ] PASS - Deck uploads and appears in list
- [ ] FAIL - Describe issue: _______________
- Upload time for test deck: _______ seconds
- Deck name displayed correctly: [ ] Yes [ ] No

### Test 3: Deck List

| Step | Action | Expected Result |
|------|--------|-----------------|
| 3.1 | Select "Study" from main menu | Deck list appears |
| 3.2 | Observe deck info | Name, card count, due count visible |
| 3.3 | Navigate between decks (if multiple) | Selection moves correctly |
| 3.4 | Select a deck | Review session starts |

**Report**:
- [ ] PASS - Deck list works correctly
- [ ] FAIL - Describe issue: _______________

### Test 4: Review Session (Core Functionality)

| Step | Action | Expected Result |
|------|--------|-----------------|
| 4.1 | Start review | Card front (question) displayed |
| 4.2 | Press button to reveal | Card back (answer) displayed |
| 4.3 | Observe rating buttons | "Again", "Hard", "Good", "Easy" visible |
| 4.4 | Rate card as "Good" | Next card appears |
| 4.5 | Complete several cards | Progress updates |
| 4.6 | Finish all due cards | Session complete screen |

**Report**:
- [ ] PASS - Review flow works correctly
- [ ] FAIL - Describe issue: _______________

**Text rendering**:
- [ ] Text readable and properly sized
- [ ] Long text wraps correctly
- [ ] Special characters display correctly (if applicable)

### Test 5: SM-2 Scheduling

This tests spaced repetition logic.

| Step | Action | Expected Result |
|------|--------|-----------------|
| 5.1 | Review a new card, rate "Good" | Card scheduled for ~1 day |
| 5.2 | Review same card again (next day), rate "Good" | Interval increases (~4-6 days) |
| 5.3 | Rate a card "Again" | Card resets to short interval |
| 5.4 | Rate a card "Easy" | Interval jumps significantly |

**Note**: Full SM-2 testing requires multiple sessions over days. For initial test, verify intervals shown make sense.

**Report**:
- [ ] PASS - Scheduling intervals appear reasonable
- [ ] FAIL - Describe issue: _______________

### Test 6: Session Statistics

| Step | Action | Expected Result |
|------|--------|-----------------|
| 6.1 | Complete a review session | Session complete screen appears |
| 6.2 | Observe statistics | Cards reviewed, correct %, time shown |
| 6.3 | Press button to continue | Returns to deck list or main menu |

**Report**:
- [ ] PASS - Stats display correctly
- [ ] FAIL - Describe issue: _______________

### Test 7: Return to CrossPoint

| Step | Action | Expected Result |
|------|--------|-----------------|
| 7.1 | From main menu, select "Return to CrossPoint" | Confirmation prompt (if any) |
| 7.2 | Confirm | Device reboots |
| 7.3 | Wait for reboot | CrossPoint main firmware loads |

**Report**:
- [ ] PASS - Returns to CrossPoint successfully
- [ ] FAIL - Describe issue: _______________

---

## Edge Case Tests

### Test 8: Empty Deck

| Step | Action | Expected Result |
|------|--------|-----------------|
| 8.1 | Upload a deck with 0 due cards | Deck appears in list |
| 8.2 | Try to study | Message: "No cards due" or similar |

### Test 9: Large Text

| Step | Action | Expected Result |
|------|--------|-----------------|
| 9.1 | Create card with very long text (500+ chars) | Text wraps, scrollable or truncated gracefully |

### Test 10: Interrupted Session

| Step | Action | Expected Result |
|------|--------|-----------------|
| 10.1 | Start review, complete some cards | Progress being made |
| 10.2 | Power off device mid-session | Device powers off |
| 10.3 | Power on, return to Anki app | Progress preserved (or session restarted gracefully) |

### Test 11: Multiple Decks

| Step | Action | Expected Result |
|------|--------|-----------------|
| 11.1 | Upload 3+ different decks | All appear in deck list |
| 11.2 | Study from different decks | Each deck tracks progress independently |

---

## Error Handling Tests

### Test 12: Invalid File Upload

| Step | Action | Expected Result |
|------|--------|-----------------|
| 12.1 | Try to upload a .txt file | Error message, upload rejected |
| 12.2 | Try to upload a corrupted .apkg | Error message, handled gracefully |

### Test 13: WiFi Disconnect During Upload

| Step | Action | Expected Result |
|------|--------|-----------------|
| 13.1 | Start uploading a large deck | Upload in progress |
| 13.2 | Disconnect WiFi mid-upload | Error message, no crash |

---

## Performance Tests

### Test 14: Upload Performance

| Deck Size | Cards | Upload Time | Notes |
|-----------|-------|-------------|-------|
| Small | ~50 | _____ sec | |
| Medium | ~500 | _____ sec | |
| Large | ~2000 | _____ sec | |

### Test 15: Review Performance

| Metric | Result | Notes |
|--------|--------|-------|
| Time to display card | _____ ms | |
| Time to flip card | _____ ms | |
| Time to load next card | _____ ms | |
| E-ink refresh quality | [ ] Clean [ ] Ghosting | |

### Test 16: Memory Usage

| Scenario | Behavior | Notes |
|----------|----------|-------|
| Large deck (2000+ cards) | [ ] Works [ ] Slow [ ] Crash | |
| Multiple review sessions | [ ] Stable [ ] Memory leak | |

---

## UI/UX Feedback

### Display Quality

| Element | Rating (1-5) | Notes |
|---------|--------------|-------|
| Font readability | | |
| Button visibility | | |
| Contrast/clarity | | |
| Layout spacing | | |

### Navigation

| Aspect | Rating (1-5) | Notes |
|--------|--------------|-------|
| Menu intuitiveness | | |
| Button responsiveness | | |
| Back/cancel options | | |
| Error messages clarity | | |

### Overall Experience

| Question | Response |
|----------|----------|
| Would you use this app regularly? | |
| What's the #1 improvement needed? | |
| Any unexpected behaviors? | |
| Comparison to Anki mobile/desktop? | |

---

## Bug Report Template

If you find a bug, please report using this format:

```
### Bug Title
[Brief description]

### Steps to Reproduce
1. 
2. 
3. 

### Expected Behavior
[What should happen]

### Actual Behavior
[What actually happens]

### Environment
- CrossPoint firmware version: 
- Anki app version: v0.1.0
- Deck used: 
- Battery level: 

### Screenshots/Photos
[If applicable]

### Additional Context
[Any other relevant info]
```

---

## Test Session Summary

**Tester**: _________________
**Date**: _________________
**Device Serial** (optional): _________________

### Results Summary

| Test | Status | Notes |
|------|--------|-------|
| 1. Main Menu | [ ] Pass [ ] Fail | |
| 2. Deck Upload | [ ] Pass [ ] Fail | |
| 3. Deck List | [ ] Pass [ ] Fail | |
| 4. Review Session | [ ] Pass [ ] Fail | |
| 5. SM-2 Scheduling | [ ] Pass [ ] Fail | |
| 6. Session Stats | [ ] Pass [ ] Fail | |
| 7. Return to CrossPoint | [ ] Pass [ ] Fail | |
| 8. Empty Deck | [ ] Pass [ ] Fail | |
| 9. Large Text | [ ] Pass [ ] Fail | |
| 10. Interrupted Session | [ ] Pass [ ] Fail | |
| 11. Multiple Decks | [ ] Pass [ ] Fail | |
| 12. Invalid File | [ ] Pass [ ] Fail | |
| 13. WiFi Disconnect | [ ] Pass [ ] Fail | |

### Overall Assessment

- [ ] Ready for daily use
- [ ] Usable with minor issues
- [ ] Major issues need fixing
- [ ] Not functional

### Top 3 Issues Found
1. 
2. 
3. 

### Top 3 Positive Observations
1. 
2. 
3. 

---

## Submitting Feedback

Please submit your completed test results via:

1. **GitHub Issues**: https://github.com/DChells/crosspoint-x4-anki/issues
2. **Direct message** to maintainer

Thank you for testing!
