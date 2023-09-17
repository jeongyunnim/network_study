---
종류: 문헌
날짜: 2023-09-13
인물_저자: 장혜식
태그: 프로그래밍,네트워크
출처: 마이크로소프트웨어 9월호
구분_장소_분류: 
aliases:
---

### 날짜: 2023-09-12

### 태그: #프로그래밍/네트워크

>[!메모]
> 

### 학습 내용
---
## 최강의 poller를 찾아라! KQueue
### select 또는 poll
- 한 프로세스에서 non blocking I/O를 처리해주는 함수.
- blocking 되는 함수를 사용하지 못한다는 문제가 있음.
- 서버의 자원을 최대로 활용할 수 있다는 장점이 있다.
### 일을 효과적으로 하는 사람과 그렇지 못한 사람.
- 비동기 소켓(asynchronous socket)
	- 송/수신 작업이 다른 이벤트를 기다리면서 처리하면 안 된다.
- 효과적인 polling을 위한 여러가지 방법
	- poller(**multiplexer**)가 이벤트를 전달하고 이벤트를 기다리는 핵심 역할을 한다.
- POSIX에서 제공하는 select()의 선언
```c++
#define FD_SETSIZE 1024
typedef unsigned long fd_mask;
#define NFDBITS (sizeof(fd_mask) * NBBY*) /* bits per mask */

typedef struct fd_set
{
	fd_mask fds_bits[howmany(FD_SEETSIZE, NFDBITS)];
} fd_set;

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
```
- [[9. Chapter12 IO 멀티 플렉싱 |select()]]에 대한 설명,
- nfds(number fds, aka max fd)를 제한하여 루프를 돌도록 한다.
	- 2, 3, 450을 감시하려면 0~450의 루프를 돌아야 한다.
### KQueue의 탄생
- 접속자 1명이 한 줄을 보내면 서버측의 fd하나에 read 이벤트가 발생한다.
	- 서버에서는 감시 중인 fd 수천 개의 중 이벤트가 발생한 하나의 fd에 도착한다.
	- read 이벤트가 발생한 것들을 모두 찾아서 처리해주고 write 해야할 것들을 셋팅해준다.
	- 하나의 이벤트를 처리하기 위해 커널에서 수천 개의 fd복사를 해온다.
- kqueue라는 대안
	- **fd의 변화를 찾는 것이 목적**
	- 프로그램에서 모니터링할 fd들을 커널에서 관리한다.
	- 이후 사용자가 추가한 fd들을 커널에 등록한 뒤 kqueue()호출을 하면 변화된 fd의 리스트가 queueing 된다.
- kqueue의 프로토타입
```c++
int kqueue(void);
int kevent(int kq, 
		   const sturct kevent *changelist, 
		   int nchanges, 
		   struct kevent *eventlist, 
		   int nevents, 
		   const struct timespec *timeout)
```
- kevent를 통해서 kqueue를 변경한다.
	- 변화를 감시할 fd를 커널에 직접 알려줌으로써 커널과 프로그램 사이의 트래픽이 절약된다.
	- [[kevent]]
---
### 생각 (파생된 질문/생각)
- 
### 출 처
- 마이크로소프트웨어 9월호
- 

### 연결 문서: {연결 이유}
- 