/* Includes ------------------------------------------------------------------*/
#include <assert.h>
#include "ring_buffer.h"
//#include "core.h"

bool RingBuffer_Init(RingBuffer *ringBuffer, char *dataBuffer, size_t dataBufferSize) 
{
//	assert(ringBuffer);
//	assert(dataBuffer);
//	assert(dataBufferSize > 0);
	
	if ((ringBuffer) && (dataBuffer) && (dataBufferSize > 0)) {
	  ringBuffer -> p_buff = dataBuffer;
		ringBuffer -> buff_size = dataBufferSize;
		ringBuffer -> p_head = dataBuffer;
		ringBuffer -> p_tail = dataBuffer;
		ringBuffer -> overflow = false;
		return true;
	}
	
	return false;
}

bool RingBuffer_Clear(RingBuffer *ringBuffer)
{
//	assert(ringBuffer);
	
	if (ringBuffer) {
		ringBuffer -> p_head = ringBuffer -> p_buff;
		ringBuffer -> p_tail = ringBuffer -> p_buff;
		ringBuffer->overflow = false;
		return true;
		
	}
	
	return false;
}

bool RingBuffer_IsEmpty(const RingBuffer *ringBuffer)
{
//  assert(ringBuffer);	
	
  if ((ringBuffer->p_head == ringBuffer->p_tail) && (ringBuffer->overflow == false))
	  return true;
  else
	  return false;
}

size_t RingBuffer_GetLen(const RingBuffer *ringBuffer)
{
//	assert(ringBuffer);
	
	if (ringBuffer) {
		
		if (ringBuffer -> overflow == true)
            return (RingBuffer_GetCapacity(ringBuffer) - ((size_t)ringBuffer->p_tail - (size_t)ringBuffer->p_head)) ;
        else
            return  ((ringBuffer->p_head - ringBuffer->p_tail)) ;

	}
	return 0;
	
}

size_t RingBuffer_GetCapacity(const RingBuffer *ringBuffer)
{
//	assert(ringBuffer);
	
	if (ringBuffer) {
		return ringBuffer -> buff_size;
	}
	return 0;	
}


bool RingBuffer_PutChar(RingBuffer *ringBuffer, char c)
{
//	assert(ringBuffer);
	
	if (ringBuffer) {
		if ((ringBuffer->p_head == ringBuffer->p_tail) && (ringBuffer -> overflow == true))
			return false;

		*(ringBuffer -> p_head) = c;
		ringBuffer -> p_head++;
		
		if (ringBuffer->p_head == (ringBuffer->p_buff + (unsigned int)ringBuffer->buff_size)) {
			ringBuffer->p_head = ringBuffer->p_buff;
			ringBuffer->overflow = true;
		}
		
		return true;
	}
	
	return false;
}

bool RingBuffer_GetChar(RingBuffer *ringBuffer, char *c)
{
//	assert(ringBuffer);
//	assert(c);
	
  if ((ringBuffer) && (c)) {
	  if ((ringBuffer->p_head == ringBuffer->p_tail) && (ringBuffer->overflow == false))
		  return false;

		*c = *(ringBuffer -> p_tail);
		ringBuffer -> p_tail++;
		if (ringBuffer->p_tail == (ringBuffer->p_buff + (unsigned int)ringBuffer->buff_size)) {
			ringBuffer->p_tail = ringBuffer->p_buff;
			ringBuffer->overflow = false;
		}

		return true;
		
	}
	return false;
}
