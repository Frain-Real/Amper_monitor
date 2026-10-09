#pragma once
template<typename T, int Size>
class RingBuffer {
private:
    T buf[Size+1];
    int head = 0, tail = 0;
    bool busy = false;

    int datalen(){
        if(this->tail >= this->head) return (this->tail - this->head);
        else return (Size - this->head + this->tail);
    }


    int hasbyte(T byte){
        if(this->head == this->tail) return -1; // no data in buffer
        int startidx = this->head;
        if(this->head > this->tail){ //
            for(int found = this->head; found < Size; ++found)
                if(this->buf[found] == byte) return found;
            startidx = 0;
        }
        for(int found = startidx; found < this->tail; ++found)
            if(this->buf[found] == byte) return found;
        return -1;
    }

    void mcpy(T *targ, const T *src, int l){
        while(l--) *targ++ = *src++;
    }

    inline void incr(volatile int *what, int n){
        *what += n;
        if(*what >= Size) *what -= Size;
    }

    int read(T *s, int len){
        int l = datalen();
        if(!l) return 0;
        if(l > len) l = len;
        int _1st = Size - this->head;
        if(_1st > l) _1st = l;
        if(_1st > len) _1st = len;
        mcpy(s, this->buf + this->head, _1st);
        if(_1st < len && l > _1st){
            mcpy(s+_1st, this->buf, l - _1st);
            incr(&this->head, l);
            return l;
        }
        incr(&this->head, _1st);
        return _1st;
    }  
    
    int write(const T *str, int l){
        int r = Size - 1 - datalen(); // rest length
        if(l > r || !l) return 0;
        int _1st = Size - this->tail;
        if(_1st > l) _1st = l;
        mcpy(this->buf + this->tail, str, _1st);
        if(_1st < l){ // add another piece from start
            mcpy(this->buf, str+_1st, l-_1st);
        }
        incr(&tail, l);
        return l;
    }

    int readto(T byte, T *s, int len){
        int idx = hasbyte(byte);
        if(idx < 0) return 0;
        int partlen = idx + 1 - this->head;
        if(idx < this->head) partlen += Size;
        if(partlen > len) return -read(s, len);
        return read(s, partlen);
    }
public:
    // stored data length
    int Datalen(){
        if(this->busy) return -1;
        this->busy = 1;
        int l = datalen();
        this->busy = 0;
        return l;
    }

    int Hasbyte(T byte){
        if(this->busy) return -1;
        this->busy = 1;
        int ret = hasbyte(byte);
        this->busy = 0;
        return ret;
    }

    int Read(T *s, int len){
        if(this->busy) return -1;
        this->busy = 1;
        int r = read(s, len);
        this->busy = 0;
        return r;
    }

    int Readto(T byte, T *s, int len){
        if(this->busy) return -1;
        this->busy = 1;
        int n = readto(byte, s, len);
        this->busy = 0;
        return n;
    }

    int Write(const T *str, int l){
        if(this->busy) return -1;
        this->busy = 1;
        int w = write(str, l);
        this->busy = 0;
        return w;
    }

    int Clear(){
        if(this->busy) return -1;
        this->busy = 1;
        this->head = 0;
        this->tail = 0;
        this->busy = 0;
        return 1;
    }
};
