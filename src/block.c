#include "block.h"

void readsb(superblock *sb){

    //simulate read from block device
    block *bp = getcache(1);
    if((bp->flag & VALID) == 0){
        readblock(bp);
    }
    memmove(sb, bp->data, sizeof(superblock));
    freecache(bp);
}

void writesb(superblock *sb){
    //simulate write into block device
    block *bp = getcache(1);
    memmove(bp->data, (char *)sb, sizeof(superblock));
    bp->size = sizeof(superblock);
    writeblock(bp);
    freecache(bp);
}

void writebitmap(char * block_device){
    block *bp = getcache(2);
    bp->size = BSIZE;
    memmove(bp->data, block_device, BSIZE);
    writeblock(bp);
    freecache(bp);
}

void writeinodetable(inodetable *itlb){
    block *bp = getcache(3);
    bp->size = BSIZE;
    memmove(bp->data, (char *)itlb, sizeof(inodetable));
    writeblock(bp);
    freecache(bp);
}

void createsb(superblock *sb){
    sb->size = 4096 * 4096;
    sb->nblocks = 4096;
    sb->ninodes = BSIZE / sizeof(inode);
    sb->blockstart = 4;
    sb->bitmap = 2;
    sb->inodetable = 3;
    sb->flag = 0; 
    pthread_mutex_init(&(sb->lock), NULL);
    writesb(sb);
}

void createbitmap(char *block_device){
    memset(block_device, 0, sizeof(block_device));
    block_device[0/8] |= (1 << (0 % 8)); //boot
    block_device[1/8] |= (1 << (1 % 8)); //sb
    block_device[2/8] |= (1 << (2 % 8)); //bitmap
    block_device[3/8] |= (1 << (3 % 8)); //inodetable
    writebitmap(block_device);
}

void createinodetable(inodetable *itlb){
    for(int i = 0; i < NINODE; i ++){
        itlb->tlb[i].type = 0;
        itlb->tlb[i].inum = i;
        itlb->tlb[i].ref = 0;
    }
    writeinodetable(itlb);
}

void readbitmap(char *block_device){
    block *bp = getcache(2);
    if((bp->flag & VALID) == 0){
        readblock(bp);
    }
    memmove(block_device, bp->data, BSIZE);
    freecache(bp);
}

void readinodetable(inodetable *itlb){
    block *bp = getcache(3);
    if((bp->flag & VALID) == 0){
        readblock(bp);
    }
    memmove((char *)itlb, bp->data, sizeof(inodetable));
    freecache(bp);
}

void initcache(cache *lrucache){
    lrucache->head.prev = &lrucache->head;
    lrucache->head.next = &lrucache->head;
    block *bp;
    for(bp = lrucache->buf; bp < lrucache->buf + BUFSIZE; bp++){
        bp->ref = 0;
        bp->next = lrucache->head.next;
        bp->prev = &lrucache->head;
        lrucache->head.next->prev = bp;
        lrucache->head.next = bp;
    }
}

block *getcache(uint blockno){
    block *bp;
    for(bp = lrucache.head.next; bp != &lrucache.head; bp = bp->next){
        if(bp->blockno == blockno){
            bp->ref++;
            return bp;
        }
    }

    for(bp = lrucache.head.prev; bp != &lrucache.head; bp = bp->prev){
        if(bp->ref == 0 && (bp->flag & DIRTY) == 0){
            bp->blockno = blockno;
            bp->flag = 0;
            bp->ref = 1;
            return bp;
        }
    }

    //cache full
    int full = 1;
    for(bp = lrucache.head.next; bp != &lrucache.head; bp = bp->next){
        if(bp->ref == 0 && (bp->flag & DIRTY) == DIRTY){
            writeblock(bp);
            bp->flag = 0;
            full = 0;
        }
    }
    if(!full){
        for(bp = lrucache.head.next; bp != &lrucache.head; bp = bp->next){
            if(bp->ref == 0 && (bp->flag & DIRTY) == 0){
                bp->blockno = blockno;
                bp->flag = 0;
                bp->ref = 1;
                return bp;
            }
        }
    }
    printf("getcache");
}

void freecache(block *bp){
    bp->ref--;
    if(bp->ref == 0){
        bp->next->prev = bp->prev;
        bp->prev->next = bp->next;
        bp->next = lrucache.head.next;
        bp->prev = &lrucache.head;
        lrucache.head.next->prev = bp;
        lrucache.head.next = bp;
    }
}

void init(){
    initcache(&lrucache);
    readsb(&sb);
    readbitmap(block_device);
    readinodetable(&itlb);
}

void create(){
    initcache(&lrucache);
    createsb(&sb);
    createbitmap(block_device);
    createinodetable(&itlb);
}

void readblock(block *bp){
    //simulate read block
    char name[50] = "/home/mashaonan/Desktop/course/fs/block/";
    int len = strlen(name);
    name[len + 0] = '0' + ((bp->blockno / 10000) % 10);
    name[len + 1] = '0' + ((bp->blockno / 1000) % 10);
    name[len + 2] = '0' + ((bp->blockno / 100) % 10);
    name[len + 3] = '0' + ((bp->blockno / 10) % 10);
    name[len + 4] = '0' + ((bp->blockno / 1) % 10);
    name[len + 5] = 0;
    FILE *fp = fopen(name,"rb");
    size_t n = fread(bp->data, BSIZE, 1, fp);
    if(n != 1){
        printf("readblock wrong\n");
        return;
    }
    bp->flag |= VALID;
    fclose(fp);
}

void writeblock(block *bp){
    char name[50] = "/home/mashaonan/Desktop/course/fs/block/";
    int len = strlen(name);
    name[len + 0] = (char)('0' + ((bp->blockno / 10000) % 10));
    name[len + 1] = (char)('0' + ((bp->blockno / 1000) % 10));
    name[len + 2] = (char)('0' + ((bp->blockno / 100) % 10));
    name[len + 3] = (char)('0' + ((bp->blockno / 10) % 10));
    name[len + 4] = (char)('0' + ((bp->blockno / 1) % 10));
    name[len + 5] = 0;
    len = strlen(name);
    FILE *fp = fopen(name,"wb");
    size_t n = fwrite(bp->data, BSIZE, 1, fp);
    if(n != 1){
        printf("writeblock wrong\n");
        return;
    }
    bp->flag &= ~DIRTY;
    fclose(fp);
}
    
uint balloc(){
    for(uint i = 0; i < 4096; i++){
        if((block_device[i / 8] & (1<<(i%8))) == 0){
            block_device[i / 8] |= (1<<(i%8));
            block *bp = getcache(i);
            memset(bp->data, 0, sizeof(bp->data));
            writebitmap(block_device);
            writeblock(bp);
            return i;
        }
    }
}

