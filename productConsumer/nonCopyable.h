#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__

namespace dd
{
    class nonCopyable
    {
    protected:
        nonCopyable() {}
        ~nonCopyable() {}
        // 禁止复制
        nonCopyable(const nonCopyable &) = delete;
        nonCopyable &operator=(const nonCopyable &) = delete;
    };

}
#endif