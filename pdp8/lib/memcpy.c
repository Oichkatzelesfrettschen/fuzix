#include "head.h" // Or specific types header

/*
 * memcpy for PDP-8 FUZIX
 * Copies 'n' bytes from 'src_packed' to 'dest_packed'.
 * 'src_packed' and 'dest_packed' are word pointers to packed char data.
 * 'n' is the number of bytes (not words).
 *
 * This is a conceptual and highly unoptimized version.
 * Real implementation needs to be very careful about:
 * - Alignment of src and dest (byte or word aligned start?)
 * - Odd/even number of bytes
 * - Overlapping regions (memcpy standard says behavior is undefined for overlap, use memmove)
 */
void *memcpy(void *dest_packed, const void *src_packed, unsigned int n) {
    uint16_t *d_word = (uint16_t *)dest_packed;
    const uint16_t *s_word = (const uint16_t *)src_packed;

    uint16_t temp_s_word, temp_d_word;
    uint8_t byte_val;
    unsigned int i;

    // Simplistic byte-by-byte copy using GET/PUT macros
    // Assumes n is number of bytes.
    // This doesn't handle byte-level start addresses within a word well.
    // A robust version would need to manage source/dest byte phases (left/right).

    for (i = 0; i < n; ++i) {
        // Determine which word and which byte (LB/RB) from source
        temp_s_word = s_word[i / 2]; // Integer division gives word index

        if ((i % 2) == 0) { // Even byte index: Left Byte
            byte_val = GET_LB(temp_s_word);
        } else { // Odd byte index: Right Byte
            byte_val = GET_RB(temp_s_word);
        }

        // Determine which word and which byte (LB/RB) for destination
        // Need to read destination word first if only modifying one byte of it.
        temp_d_word = d_word[i / 2];
        if ((i % 2) == 0) { // Even byte index: Left Byte
            PUT_LB(temp_d_word, byte_val);
        } else { // Odd byte index: Right Byte
            PUT_RB(temp_d_word, byte_val);
        }
        d_word[i / 2] = temp_d_word; // Write modified word back
    }

    return dest_packed;
}

// Other lib functions like strlen, strcpy, memset would follow similar logic,
// using GET_LB, PUT_LB, GET_RB, PUT_RB.
