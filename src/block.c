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
    bp->flag |= DIRTY;
    freecache(bp);
}


void writebitmap(char * block_device){
    block *bp = getcache(2);
    bp->size = BSIZE;
    memmove(bp->data, block_device, BSIZE);
    bp->flag |= DIRTY;
    freecache(bp);
}

void writeinodetable(inodetable *itlb){
    block *bp = getcache(3);
    bp->size = BSIZE;
    memmove(bp->data, (char *)itlb, BSIZE);
    bp->flag |= DIRTY;
    freecache(bp);
}

void createsb(superblock *sb){
    sb->size = 4069 * 8;
    sb->nblocks = 4096 * 8 - 4;
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
    block_device[0/8] |= (1 << (0 % 8));
    block_device[1/8] |= (1 << (1 % 8));
    block_device[2/8] |= (1 << (2 % 8));
    block_device[3/8] |= (1 << (3 % 8));
    writebitmap(block_device);
}

void createinodetable(inodetable *itlb){
    for(int i = 0; i < NINODE; i ++){
        itlb->tlb[i].type = 0;
        itlb->tlb[i].inum = i + 1;
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
    memmove((char *)itlb, bp->data, BSIZE);
    freecache(bp);
}

void initcache(cache *lrucache){
    lrucache->head->prev = lrucache->head;
    lrucache->head->next = lrucache->head;
    block *bp;
    for(bp = lrucache->buf; bp < lrucache->buf + BUFSIZE; bp++){
        bp->ref = 0;
        bp->next = lrucache->head->next;
        bp->prev = lrucache->head;
        lrucache->head->next->prev = bp;
        lrucache->head->next = bp;
    }
}

block *getcache(uint blockno){
    block *bp;
    for(bp = lrucache.head->next; bp != lrucache.head; bp = bp->next){
        if(bp->blockno == blockno){
            bp->ref++;
            return bp;
        }
    }

    for(bp = lrucache.head->prev; bp != lrucache.head; bp = bp->prev){
        if(bp->ref == 0 && (bp->flag & DIRTY) == 0){
            bp->blockno = blockno;
            bp->flag = 0;
            bp->ref = 1;
            return bp;
        }
    }

    //cache full
    int full = 1;
    for(bp = lrucache.head->next; bp != lrucache.head; bp = bp->next){
        if(bp->ref == 0 && (bp->flag & DIRTY) == DIRTY){
            writeblock(bp);
            bp->flag = 0;
            full = 0;
        }
    }
    if(!full){
        for(bp = lrucache.head->next; bp != lrucache.head; bp = bp->next){
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
        bp->next = lrucache.head->next;
        bp->prev = lrucache.head;
        lrucache.head->next->prev = bp;
        lrucache.head->next = bp;
    }
}

void init(){
    readsb(&sb);
    readbitmap(block_device);
    readinodetable(&itlb);
    initcache(&lrucache);
}

void create(){
    createsb(&sb);
    createbitmap(block_device);
    createinodetable(&itlb);
    initcache(&lrucache);
}

void readblock(block *bp){
    //simulate read block
    char *name = "block/";
    int len = strlen(name);
    name[len + 0] = '0' + ((bp->blockno / 10000) % 10);
    name[len + 1] = '0' + ((bp->blockno / 1000) % 10);
    name[len + 2] = '0' + ((bp->blockno / 100) % 10);
    name[len + 3] = '0' + ((bp->blockno / 10) % 10);
    name[len + 4] = '0' + ((bp->blockno / 1) % 10);
    name[len + 5] = 0;
    FILE *fp = fopen(name,"r");
    size_t n = fread(bp->data, BSIZE, 1, fp);
    bp->size = n;
    bp->flag |= VALID;
    fclose(fp);
}

void writeblock(block *bp){
    char *name = "block/";
    int len = strlen(name);
    name[len + 0] = '0' + ((bp->blockno / 10000) % 10);
    name[len + 1] = '0' + ((bp->blockno / 1000) % 10);
    name[len + 2] = '0' + ((bp->blockno / 100) % 10);
    name[len + 3] = '0' + ((bp->blockno / 10) % 10);
    name[len + 4] = '0' + ((bp->blockno / 1) % 10);
    name[len + 5] = 0;
    FILE *fp = fopen(name,"w");
    size_t n = fwrite(bp->data, bp->size, 1, fp);
    bp->flag &= ~DIRTY;
    fclose(fp);
}
    
uint balloc(){
    for(uint i = 0; i < 4096 * 8; i++){
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

