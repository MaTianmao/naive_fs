#include "file.h"

uint bmap(inode *ino, uint bn){
    uint addr;
    if(bn < DIRECT){
        if((addr = ino->direct[bn]) == 0){
            ino->direct[bn] = addr = balloc();
            writeinodetable(&itlb);
        }
        return addr;
    }
    bn -= DIRECT;
    if(bn < SINDIRECT){
        if((addr = ino->direct[DIRECT]) == 0){
            ino->direct[DIRECT] = addr = balloc();
            writeinodetable(&itlb);
        }
        block *bp = getcache(addr);
        if((bp->flag & VALID) == 0){
            readblock(bp);
        }
        uint *a = (uint *)bp->data;
        if((addr = a[bn]) == 0){
            a[bn] = addr = balloc();
            bp->flag |= DIRTY;
        }
        freecache(bp);
        return addr;
    }
    bn -= SINDIRECT;
    if(bn < DINDIRECT){
        if((addr = ino->direct[DIRECT + 1]) == 0){
            ino->direct[DIRECT + 1] = addr = balloc();
            writeinodetable(&itlb);
        }
        block *bp = getcache(addr);
        if((bp->flag & VALID) == 0){
            readblock(bp);
        }
        uint *a = (uint *)bp->data;
        if((addr = a[bn / (BSIZE / sizeof(uint))]) == 0){
            a[bn / (BSIZE / sizeof(uint))] = addr = balloc();
            bp->flag |= DIRTY;
        }
        freecache(bp);
        bp = getcache(addr);
        if((bp->flag & VALID) == 0){
            readblock(bp);
        }
        a = (uint *)bp->data;
        if((addr = a[bn % (BSIZE / sizeof(uint))]) == 0){
            a[bn % (BSIZE / sizeof(uint))] = addr = balloc();
            bp->flag |= DIRTY;
        }
        freecache(bp);
        return addr;
    }
    
    printf("bmap: out of range");
}

int readinode(inode *ino, char *dst, uint off, uint n){
    if(off > ino->size || n < 0 || dst == NULL){
        printf("readinode: off out of range or n < 0 or dst is NULL");
        return 0;
    }
    if(off + n > ino->size)
        n = ino->size - off;
    uint m = BSIZE;
    for(int i = 0; i < n; i += m, off += m, dst += m){
        block *bp = getcache(bmap(ino, off / BSIZE));
        if((bp->flag & VALID) == 0){
            readblock(bp);
        }
        m = min(n - i, BSIZE - off % BSIZE);
        memmove(dst, bp->data + off % BSIZE, m);
        freecache(bp);
    }   
    return n;
}

int writeinode(inode *ino, char *dst, uint off, uint n){
    if(off > ino->size || n < 0 || dst == NULL){
        printf("writeinode");
        return 0;
    }
    if(off + n > 4 * 1024 * 1024){
        printf("out of 4MB");
        return 0;
    }
    uint m = BSIZE;
    for(int i = 0; i < n; i += m, off += m, dst += m){
        block *bp = getcache(bmap(ino, off / BSIZE));
        if((bp->flag & VALID) == 0){
            readblock(bp);
        }
        m = min(n-i, BSIZE - off % BSIZE);
        bp->size = m + off % BSIZE;
        memmove(bp->data + off % BSIZE, dst, m);
        bp->flag |= DIRTY;
        freecache(bp);
    }
    ino->size = off;
    writeinodetable(&itlb);
    return n;
}

inode *ialloc(uint type){
    for(int i = 1; i < NINODE; i++){
        if(itlb.tlb[i].type == 0){
            memset(&itlb.tlb[i], 0, sizeof(inode));
            itlb.tlb[i].type = type;
            writeinodetable(&itlb);
            return &itlb.tlb[i];
        }
    }
}

void ifree(inode *ino){
    memset(ino, 0, sizeof(inode));
    writeinodetable(&itlb);
}