#if !defined(RESIP_FDPOLL_HXX)
#define RESIP_FDPOLL_HXX

#include "rutil/Socket.hxx"

/* The Makefile system should define one of the following:
 * HAVE_EPOLL: system call epoll() is available
 * If none are defined then classes will still be defined, but
 * calls to them will assert.
 */

#if defined(HAVE_EPOLL)
#define RESIP_POLL_IMPL_EPOLL  // Only one currently implemented
#endif

namespace resip {


typedef unsigned short FdPollEventMask;
#define FPEM_Read	0x0001	// POLLIN
#define FPEM_Write	0x0002	// POLLOUT
#define FPEM_Error	0x0004	// POLLERR	(select exception)
#define FPEM_Edge	0x4000	// EPOLLET

class FdPollGrp;


class FdPollItemIf 
{
  //friend class FdPollGrp;
  public:
    FdPollItemIf() { };
    virtual ~FdPollItemIf();

    virtual Socket getPollSocket() const = 0;

    /**
        Called by PollGrp when activity is possible
    **/
    virtual void processPollEvent(FdPollEventMask mask) = 0;
};

class FdPollItemBase : public FdPollItemIf 
{
  //friend class FdPollGrp;
  public:
    FdPollItemBase(FdPollGrp *grp, Socket fd, FdPollEventMask mask);
    virtual ~FdPollItemBase();

    virtual Socket	getPollSocket() const { return mPollSocket; }

 protected:

    FdPollGrp*		mPollGrp;
    Socket		mPollSocket;
    FdPollEventMask	mPollMask;		// events we want
};

class FdPollGrp 
{
  public:
    FdPollGrp();
    virtual ~FdPollGrp();

    /// factory
    static FdPollGrp* create();


    virtual void addPollItem(FdPollItemIf *item, FdPollEventMask newMask) = 0;
    virtual void modPollItem(const FdPollItemIf *item, FdPollEventMask newMask) = 0;
    virtual void delPollItem(FdPollItemIf *item) = 0;

    virtual void process() = 0;

    /// get the epoll-fd (epoll_create()) -- it is int, not Socket
    /// This is fd (type int), not Socket. It may be -1 if epoll
    /// is not enabled.
    virtual int	getEPollFd() const = 0;

    /// Add our epoll-fd into the fdSet (for hierarchical selects)
    void buildFdSet(FdSet& fdSet) const;
    void buildFdSet(fd_set& readfds) const;
    /// process epoll queue if epoll-fd is readable in fdset
    void processFdSet(FdSet& fdset);
    void processFdSet(fd_set& readfds);

    virtual FdPollItemIf* getItemByFd(Socket fd) = 0;

    // convience functions. not sure if we should keep this or not
    FdPollItemIf* modifyEventMaskByFd(FdPollEventMask mask, Socket fd);
};



///////////////////////////////////////////////////////////////////////////////

} // namespace resip

#endif //!defined(RESIP_FDPOLL_HXX)

/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000-2005 Jacob Butcher
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 */