#include <memory.h>
#include <type.h>
#include <sys.h>
#include <string.h>
#include <monitor.h>
#include <asm/system.h>


u32int placement_addr = (u32int)KMALLOC_START;

void *kmalloc(u32int size)
{
    void *m_start;
    if (placement_addr & (0xfff))
    {
        placement_addr &= 0xfffff000;
        placement_addr += 0x1000;
    }
    m_start = (void *)placement_addr;
    placement_addr += size;
    return m_start;
}

void *kmalloc_f(u32int size, u32int *frame_idx)
{
    void *m_start;
    if (placement_addr & 0xfff)
    {
        placement_addr &= 0xfffff000;
        placement_addr += 0x1000;
    }
    *frame_idx = placement_addr / 0x1000;
    m_start = (void *)placement_addr;
    placement_addr += size;
    return m_start;
}
void init_frame()
{
    int i;
    memset(frames, 0x00, sizeof(u32int) * FRAME_BITMAP_NUM);
    for (i=0; i<(u32int)KERNEL_END; i+=0x1000)
    {
        clear_frame(i/0x1000);
    }
}

void *frame2pointer(u32int frame_idx)
{
    u32int addr;
    addr = frame_idx * 0x1000;
    return (void *)addr;
}

void framecpy(u32int dst_frame_idx, u32int src_frame_idx)
{
    u32int src, dst;
    src = src_frame_idx * 0x1000;
    dst = dst_frame_idx * 0x1000;    
    cli();
    disable_paging();
    memcpy((void *)dst, (void *)src, 0x1000);
    enable_paging();
    sti();
}

void mark_frame(u32int frame_idx)
{
    u32int b_idx = BITMAP_INDEX(frame_idx);
    u32int b_offset = BITMAP_OFFSET(frame_idx);
    frames[b_idx] |= (0x1 << b_offset);
}

void clear_frame(u32int frame_idx)
{
    u32int b_idx = BITMAP_INDEX(frame_idx);
    u32int b_offset = BITMAP_OFFSET(frame_idx);
    frames[b_idx] &= ~(0x1 << b_offset);
}

u32int test_frame(u32int frame_idx)
{
    u32int b_idx = BITMAP_INDEX(frame_idx);
    u32int b_offset = BITMAP_OFFSET(frame_idx);
    return (frames[b_idx] & (0x1 << b_offset));
}

u32int get_free_frame_idx()
{
    u32int i, j;
    for (i=0; i<FRAME_BITMAP_NUM; i++)
    {
        if (frames[i] != 0xffffffff)
        {
            for (j=0; j<32; j++)
            {
                u32int b = 0x1 << j;
                if (!(frames[i]&b))
                {
                    return i*32+j;
                }
            }
        }
    }
    panic("no free frame\n");
    return (u32int)-1;
}
