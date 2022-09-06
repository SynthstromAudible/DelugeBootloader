/*
343  *      v7_dma_inv_range(start,end)
344  *
345  *      Invalidate the data cache within the specified region; we will
346  *      be performing a DMA operation in this region and we want to
347  *      purge old data in the cache.
348  *
349  *      - start   - virtual start address of region
350  *      - end     - virtual end address of region
351  */


         .macro  dcache_line_size, reg, tmp
         mrc     p15, 0, \tmp, c0, c0, 1         @ read ctr
         lsr     \tmp, \tmp, #16
         and     \tmp, \tmp, #0xf                @ cache line size encoding
         mov     \reg, #4                        @ bytes per word
         mov     \reg, \reg, lsl \tmp            @ actual cache line size
         .endm


	.text
	.code 32

	.global v7_dma_inv_range

	.func v7_dma_inv_range

v7_dma_inv_range:
         dcache_line_size r2, r3
         sub     r3, r2, #1
         tst     r0, r3
         bic     r0, r0, r3
 #ifdef CONFIG_ARM_ERRATA_764369
         ALT_SMP(W(dsb))
         ALT_UP(W(nop))
 #endif
         mcrne   p15, 0, r0, c7, c14, 1          @ clean & invalidate D / U line

         tst     r1, r3
         bic     r1, r1, r3
         mcrne   p15, 0, r1, c7, c14, 1          @ clean & invalidate D / U line
 1:
         mcr     p15, 0, r0, c7, c6, 1           @ invalidate D / U line
         add     r0, r0, r2
         cmp     r0, r1
         blo     1b
         dsb     st
         bx     lr
	.endfunc


	/*
		299  *      v7_dma_flush_range(start,end)
		300  *      - start   - virtual start address of region
		301  *      - end     - virtual end address of region
		302  */

	.global v7_dma_flush_range

	.func v7_dma_flush_range

v7_dma_flush_range:
		dcache_line_size r2, r3
		sub     r3, r2, #1
		bic     r0, r0, r3
		#ifdef CONFIG_ARM_ERRATA_764369
		ALT_SMP(W(dsb))
		ALT_UP(W(nop))
		#endif
		1:
		mcr     p15, 0, r0, c7, c14, 1          @ clean & invalidate D / U line
		add     r0, r0, r2
		cmp     r0, r1
		blo     1b
		dsb
		mov     pc, lr
	.endfunc

	.align 4
	.end
