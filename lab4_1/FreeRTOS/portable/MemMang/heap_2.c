/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*
 * A sample implementation of pvPortMalloc() and vPortFree() that permits
 * allocated blocks to be freed, but does not combine adjacent free blocks
 * into a single larger block (and so will fragment memory).  See heap_4.c for
 * an equivalent that does combine adjacent blocks into single larger blocks.
 *
 * See heap_1.c, heap_3.c and heap_4.c for alternative implementations, and the
 * memory management pages of http://www.FreeRTOS.org for more information.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include "FreeRTOS.h"
#include "task.h"

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#if( configSUPPORT_DYNAMIC_ALLOCATION == 0 )
	#error This file must not be used if configSUPPORT_DYNAMIC_ALLOCATION is 0
#endif

/* A few bytes might be lost to byte aligning the heap start address. */
#define configADJUSTED_HEAP_SIZE	( configTOTAL_HEAP_SIZE - portBYTE_ALIGNMENT )

/*
 * Initialises the heap structures before their first use.
 */
static void prvHeapInit( void );

/* Allocate the memory for the heap. */
#if( configAPPLICATION_ALLOCATED_HEAP == 1 )
	/* The application writer has already defined the array used for the RTOS
	heap - probably so it can be placed in a special segment or address. */
	extern uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
#else
	static uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
#endif /* configAPPLICATION_ALLOCATED_HEAP */


/* Define the linked list structure.  This is used to link free blocks in order
of their size. */
typedef struct A_BLOCK_LINK
{
	struct A_BLOCK_LINK *pxNextFreeBlock;	/*<< The next free block in the list. */
	size_t xBlockSize;						/*<< The size of the free block. */
} BlockLink_t;


static const uint16_t heapSTRUCT_SIZE	= ( ( sizeof ( BlockLink_t ) + ( portBYTE_ALIGNMENT - 1 ) ) & ~portBYTE_ALIGNMENT_MASK );
#define heapMINIMUM_BLOCK_SIZE	( ( size_t ) ( heapSTRUCT_SIZE * 2 ) )

/* Create a couple of list links to mark the start and end of the list. */
static BlockLink_t xStart, xEnd;

/* Keeps track of the number of free bytes remaining, but says nothing about
fragmentation. */
static size_t xFreeBytesRemaining = configADJUSTED_HEAP_SIZE;

/* STATIC FUNCTIONS ARE DEFINED AS MACROS TO MINIMIZE THE FUNCTION CALL DEPTH. */

/*
 * Insert a block into the list of free blocks - which is ordered by size of
 * the block.  Small blocks at the start of the list and large blocks at the end
 * of the list.
 */
// every line should be end with \ even the empty line in macro
// the pxBlockToInsert is the new free space need to insert
#define prvInsertBlockIntoFreeList( pxBlockToInsert )                                                                                  \
{                                                                                                                                      \
    BlockLink_t * pxIterator,*pxBlock,*temp,*endaddress,*startaddress,*pxPrevious,*pxIterator2;                                                     \
    size_t xBlockSize,temp_size;                                                                                                                 \
                                                                                                                                       \
    xBlockSize = pxBlockToInsert->xBlockSize;                                                                                          \
    pxBlock = pxBlockToInsert;                                                                                                         \
    /*the endaddress of the pxBlockToInsert*/                                                                                          \
    endaddress = pxBlock + (xBlockSize)/8;                                                                                             \
    pxPrevious = &xStart;                                                                                                              \
                                                                                                                                       \
    /*traverse the current free block(in the ascending order)*/                                                                        \
    for (pxIterator = &xStart; pxIterator->pxNextFreeBlock != &xEnd; pxIterator = pxIterator->pxNextFreeBlock)                         \
    {                                                                                                                                  \
    	/*next BlockLink_t*/                                                                                                           \
        temp = pxIterator->pxNextFreeBlock;                                                                                            \
        /* in the merge section there is only one condition(endaddress == temp or pxBlock == startaddress)*/                           \
        if (endaddress == temp)                                                                                                        \
        {                                                                                                                              \
            /*if it fit the condition, it mean that there is a free space */                                                           \
            /*behind the pxBlock. So we can merge it by adding the free space xBlockSize to the pxBlock*/                              \
			/*and replace the pxBlock->pxNextFreeBlock by temp->pxNextFreeBlock*/                                                      \
		    pxBlock->xBlockSize = temp->xBlockSize+pxBlock->xBlockSize;                                                                                   \
		    pxPrevious->pxNextFreeBlock = temp->pxNextFreeBlock;                                                                                     \
		    break;\
        }                                                                                                                              \
                                                                                                                                       \
        /*search the free space in front of the pxBlock*/                                                                              \
		startaddress = temp + (temp->xBlockSize)/8;                                                                                    \
        /*if it fit the condition, it mean that there is a free space */                                                               \
        /*in front of the pxBlock. So we can merge it by adding the pxBlock xBlockSize to the free space*/                             \
        if (pxBlock == startaddress)                                                                                                   \
        {                                                                                                                              \
        	temp->xBlockSize = pxBlock->xBlockSize+temp->xBlockSize;                                                                                   \
        	pxPrevious->pxNextFreeBlock = temp->pxNextFreeBlock;\
        	pxBlock = temp;\
        	break;\
        }                                                                                                                              \
		pxPrevious = temp;                                                                                                             \
    }                                                                                                                                  \
    /*if it does not have free space in front of the pxBlockToInsert or behind the pxBlockToInsert, we do not merge it*/               \
	/* or we can say that we creat a new BlockLink_t(pxBlockToInsert)*/                                                                \
    /* Iterate through the list until a block is found that has a larger size */                                                   \
    /* than the block we are inserting. */                                                                                         \
    for( pxIterator = &xStart; pxIterator->pxNextFreeBlock->xBlockSize < pxBlock->xBlockSize; pxIterator = pxIterator->pxNextFreeBlock )    \
    {                                                                                                                              \
        /* There is nothing to do here - just iterate to the correct position. */                                                  \
    }                                                                                                                              \
                                                                                                                                   \
    /* Update the list to include the block being inserted in the correct */                                                       \
    /* position. */                                                                                                                \
    /* here, the pxIterator->xBlockSize is smaller than xBlockSize*/                                                               \
	/* because the property of the for loop(it will not execute the pxIterator = pxIterator->pxNextFreeBlock*/                     \
    /* when the pxIterator->pxNextFreeBlock->xBlockSize < xBlockSiz is false)*/                                                    \
	pxBlock->pxNextFreeBlock = pxIterator->pxNextFreeBlock;                                                                \
    pxIterator->pxNextFreeBlock = pxBlock;                                                                                 \
}
/*-----------------------------------------------------------*/

void *pvPortMalloc( size_t xWantedSize )
{
BlockLink_t *pxBlock, *pxPreviousBlock, *pxNewBlockLink;
static BaseType_t xHeapHasBeenInitialised = pdFALSE;
void *pvReturn = NULL;
size_t BlockSize, WantedSize;
char data[80];
WantedSize = xWantedSize;

	vTaskSuspendAll();
	{
		/* If this is the first call to malloc then the heap will require
		initialisation to setup the list of free blocks. */
		if( xHeapHasBeenInitialised == pdFALSE )
		{
			prvHeapInit();
			xHeapHasBeenInitialised = pdTRUE;
		}

		/* The wanted size is increased so it can contain a BlockLink_t
		structure in addition to the requested amount of bytes. */
		if( xWantedSize > 0 )
		{
			xWantedSize += heapSTRUCT_SIZE;

			/* Ensure that blocks are always aligned to the required number of bytes. */
			if( ( xWantedSize & portBYTE_ALIGNMENT_MASK ) != 0 )
			{
				/* Byte alignment required. */
				xWantedSize += ( portBYTE_ALIGNMENT - ( xWantedSize & portBYTE_ALIGNMENT_MASK ) );
			}
		}

		if( ( xWantedSize > 0 ) && ( xWantedSize < configADJUSTED_HEAP_SIZE ) )
		{
			/* Blocks are stored in byte order - traverse the list from the start
			(smallest) block until one of adequate size is found. */
			pxPreviousBlock = &xStart;
			pxBlock = xStart.pxNextFreeBlock;
			while( ( pxBlock->xBlockSize < xWantedSize ) && ( pxBlock->pxNextFreeBlock != NULL ) )
			{
				pxPreviousBlock = pxBlock;
				pxBlock = pxBlock->pxNextFreeBlock;
			}

			/* If we found the end marker then a block of adequate size was not found. */
			if( pxBlock != &xEnd )
			{
				/* Return the memory space - jumping over the BlockLink_t structure
				at its start. */
				pvReturn = ( void * ) ( ( ( uint8_t * ) pxPreviousBlock->pxNextFreeBlock ) + heapSTRUCT_SIZE );

				/* This block is being returned for use so must be taken out of the
				list of free blocks. */
				pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;

				/* If the block is larger than required it can be split into two. */
				if( ( pxBlock->xBlockSize - xWantedSize ) > heapMINIMUM_BLOCK_SIZE )
				{
					/* This block is to be split into two.  Create a new block
					following the number of bytes requested. The void cast is
					used to prevent byte alignment warnings from the compiler. */
					pxNewBlockLink = ( void * ) ( ( ( uint8_t * ) pxBlock ) + xWantedSize );

					/* Calculate the sizes of two blocks split from the single
					block. */
					pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;
					pxBlock->xBlockSize = xWantedSize;

					/* Insert the new block into the list of free blocks. */
					prvInsertBlockIntoFreeList( ( pxNewBlockLink ) );
				}

				xFreeBytesRemaining -= pxBlock->xBlockSize;
			}
		}

		traceMALLOC( pvReturn, xWantedSize );
	}
	( void ) xTaskResumeAll();

	#if( configUSE_MALLOC_FAILED_HOOK == 1 )
	{
		if( pvReturn == NULL )
		{
			extern void vApplicationMallocFailedHook( void );
			vApplicationMallocFailedHook();
		}
	}
	#endif

    BlockSize = xWantedSize;
    sprintf(data, "pvReturn: %p | heapSTRUCT_SIZE: %0d | WantedSize: %3d | BlockSize: %3d\n\r", pvReturn, heapSTRUCT_SIZE, WantedSize, BlockSize);
	HAL_UART_Transmit(&huart2, (uint8_t *)data, strlen(data), 0xffff);

	return pvReturn;
}
/*-----------------------------------------------------------*/

void vPortFree( void *pv )
{
uint8_t *puc = ( uint8_t * ) pv;
BlockLink_t *pxLink;

	if( pv != NULL )
	{
		/* The memory being freed will have an BlockLink_t structure immediately
		before it. */
		puc -= heapSTRUCT_SIZE;

		/* This unexpected casting is to keep some compilers from issuing
		byte alignment warnings. */
		pxLink = ( void * ) puc;

		vTaskSuspendAll();
		{
			/* Add this block to the list of free blocks. */
			prvInsertBlockIntoFreeList( ( ( BlockLink_t * ) pxLink ) );
			xFreeBytesRemaining += pxLink->xBlockSize;
			traceFREE( pv, pxLink->xBlockSize );
		}
		( void ) xTaskResumeAll();
	}
}
/*-----------------------------------------------------------*/

size_t xPortGetFreeHeapSize( void )
{
	return xFreeBytesRemaining;
}
/*-----------------------------------------------------------*/

void vPortInitialiseBlocks( void )
{
	/* This just exists to keep the linker quiet. */
}
/*-----------------------------------------------------------*/

static void prvHeapInit( void )
{
BlockLink_t *pxFirstFreeBlock;
uint8_t *pucAlignedHeap;

	/* Ensure the heap starts on a correctly aligned boundary. */
	pucAlignedHeap = ( uint8_t * ) ( ( ( portPOINTER_SIZE_TYPE ) &ucHeap[ portBYTE_ALIGNMENT ] ) & ( ~( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) ) );

	/* xStart is used to hold a pointer to the first item in the list of free
	blocks.  The void cast is used to prevent compiler warnings. */
	xStart.pxNextFreeBlock = ( void * ) pucAlignedHeap;
	xStart.xBlockSize = ( size_t ) 0;

	/* xEnd is used to mark the end of the list of free blocks. */
	xEnd.xBlockSize = configADJUSTED_HEAP_SIZE;
	xEnd.pxNextFreeBlock = NULL;

	/* To start with there is a single free block that is sized to take up the
	entire heap space. */
	pxFirstFreeBlock = ( void * ) pucAlignedHeap;
	pxFirstFreeBlock->xBlockSize = configADJUSTED_HEAP_SIZE;
	pxFirstFreeBlock->pxNextFreeBlock = &xEnd;
}
/*-----------------------------------------------------------*/

void vPrintFreeList(void)
{
	char data[100];
	// initialize the data and transmit the title
	memset(data, '\0', sizeof(data));
	sprintf(data, "StartAddress heapSTRUCT_SIZE xBlockSize     EndAddress\n\r");
	HAL_UART_Transmit(&huart2, (uint8_t *)data, strlen(data), 0xffff);

	// initialize the pointer which point to the fist freeblock (the smallest)
	// the BlockLink_t is a 8 byte struct
	// so the pointer BlockLink_t* is a pointer that point to a 8 byte struct
	BlockLink_t* pointer = xStart.pxNextFreeBlock;

	// main loop to search freeblock by their size in ascending order
	for(;;)
	{
		// if the pointer equal the xEnd, break the loop
		if (pointer == &xEnd) {break;}

		// transmit the data
		// ((pointer->xBlockSize)/8) mean that the pointer should go forward with 8 byte step
		// because the pointer BlockLink_t* is a pointer that point to a 8 byte struct
		sprintf(data, "%p         %d           %4d         %p\n\r",
				pointer,heapSTRUCT_SIZE,pointer->xBlockSize,pointer+((pointer->xBlockSize)/8));
		HAL_UART_Transmit(&huart2, (uint8_t *)data, strlen(data), 0xffff);

		// update replace the pointer with the next blocklink
		pointer = pointer->pxNextFreeBlock;
	}

	// transmit the ADJUSTED_HEAP_SIZE and xFreeBytesRemaining
	// which will be calculate in the function prvHeapInit and pvPortMalloc
	sprintf(data, "configADJUSTED_HEAP_SIZE: %0d xFreeBytesRemaining: %0d\n\r",configADJUSTED_HEAP_SIZE,xFreeBytesRemaining);
	HAL_UART_Transmit(&huart2, (uint8_t *)data, strlen(data), 0xffff);
}
