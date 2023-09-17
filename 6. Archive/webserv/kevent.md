---
종류: 문헌
날짜: 2023-09-14
태그: 프로그래밍,네트워크
출처: HTTP 완벽 가이드
구분_장소_분류: 
aliases:
---

### 날짜: 2023-09-14 19:39
### 태그: #프로그래밍/네트워크

>[!메모]
> 

### 학습 내용
---
## Kqueue 추가 정보
- kqueue를 온전히 사용하기 위해 아래의 헤더를 추가해야 한다.
```c++
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
```
- 새로운 kernel event queue를 생성하고 해당 descriptor를 반환.
- fork()된 프로세스가 상속받을 수 없다.
	- [[RFFDG플래그]]를 쓰지 않고 rfork()를 호출한다면 descriptor table을 공유할 수 있다.
- 반환 값
	- 성공: kqueue의 fd
	- 실패: -1
- 에러
	- ENOMEM 
		- 커널 큐 메모리 할당 실패
	- EMFILE
		- fd 테이블이 가득 참
	- ENFILE
		- 시스템 파일 테이블이 가득 참
## kevent
```c++
int kevent(int kq, 
		   const struct kevent *changelist, 
		   int nchanges,
		   struct kevent *eventlist,
		   int nevents, 
		   const struct timespec *timeout)
```
- kq
	- kq의 fd
- changelist
	- kqueue에 등록할 kevent 구조체 주소
- nchanges
	- 등록할 이벤트의 개수
- eventlist
	- event가 발생했을 때, 커널에서 이벤트를 넣어줄 배열
- nevents
	- 들어오는 이벤트의 최대 값
- timespec
	- 기다리는 시간
	- 정확히 무엇을 기다리는 것인가?
		- 각 소켓에 대한 입력 타이머
		- 모든 이벤트의 발생에 대한 타이머
	- 0인 경우 무한 대기한다.
### Description
- 큐에 이벤트를 등록 또는 미처리된 이벤트를 사용자에게 반환
- 큐에 이벤트를 등록
	- changelist, nchanges를 통해 일어남.
	- Read, Write, Add, Delete 중 어떤 이벤트를 시스템이 감지할 것인지 요청
	- 변화가 있을 때만 호출하여 변경시켜준다.
- 미처리된 이벤트를 얻기 위해서는 이 함수로부터 복귀 후 eventlist, nevens를 통해 알 수 있음.
	- eventlist와 changelist는 통상적으로 같은 배열을 사용한다고 되어있으나, 안전한 사용을 위해서는 두 배열을 따로 두는 것이 좋다.
- 추가되거나 변경되는 이벤트를 등록, 발생된 이벤트를 eventlist에 넣고 발생한 이벤트 수를 반환한다.
	- 이벤트 등록 시 중복되는 이벤트를 등록해서는 안된다.
	- kevent()를 호출할 때는 nchanges 값이 초기화 되어야 한다.
### Usage
- kevent의 호출은 반복해서 일어남.
	- 동기적으로 일어남
- kevent를 호출하고 복귀되면 nchanges는 0으로 초기화 해야 함.
	- changelist와 nchanges의 값이 kevent()가 호출 되기 전에 초기화 되어야 함.
	- 복귀라는 의미는 다시 이 함수로 오기 전에..?
### kevent structure init
```c++
struct kevent
{
	uintptr_t ident; /* identify event: 이벤트에 대한 identifier */
	short filter; /* identifies the kernel: 이벤트 필터 플래그 */
	u_short flags; /* actions to perform on the evnet: kqueue에 대한 액션 플래그 */
	u_int fflags; /* fliter-specific flags: 필터 플래그 */
	intptr_t data; /* filter-specific data value: 필터 데이터 */
	void *udata; /* opaque user-defined value passed through the kernel unchanged: 사용자 정의 데이터 */
}
```

### input flags
- EV_ADD
	- 이벤트 추가
	- 기존에 있었던 이벤트를 다시 추가하면 기존에 설정 되어있던 매개변수가 수정된다.
	- EV_DISABLE 플래그로 재정의될 때까지 이벤트를 활성화한다.
- EV_ENABLE
	- 이벤트 활성화
	- 이벤트 발생 시 kevent가 반환할 수 있도록 한다.
- EV_DISABLE
	- 이벤트 비활성화
	- 이벤트를 반환하지 않도록 한다.
	- 이 필터 자체는 비활성화되지 않음.
- EV_DELETE
	- 이벤트 삭제
- EV_ONESHOT
	- 설정된 이벤트가 한 번만 반환하도록 한다.
	- 사용자가 kqueue에서 이벤트를 반환받으면 삭제된다.
- EV_CLEAR
	- 사용자가 이벤트를 검색한 후 상태가 리셋된다.
	- 현재 상태를 조회하기보다 상태 전환을 보고하는 필터에 유용하다.
- 기타 플래그
	- EV_RECEIPT
	- EV_EOF
	- EV_OOBAND
	- EV_ERROR
### fflags
- EVFILT_READ
	- fd에 읽을 수 있는 데이터가 있을 때마다 반환한다. 필터의 동작은 discriptor의 유형에 따라 다르다.
		- socket
			- 들어오는 연결이 보류 중일 때 반환. 데이터에는 listen의 backlog 크기가 포함된다.
			- 다른 socket fd는 SO_RCVLOWAT 값에 따라서 읽을 데이터가 있을 때 반환된다.
				- fflags에 NOTE_LOWAT 플래그를 설정하여 low watermark를 지정한 경유 유효
			- flag에 EV_OOBAND가 있으면 읽을 수 있는 잠재적인 OOB 바이트 수와 동일한 소켓 데이터에 대역 외 데이터가 있음을 나타낸다.
			- 소켓의 읽기 버퍼가 종료된 경우 플래그에 EV_EOF를 설정하고 fflags에 소켓 오류를 반환한다.
				- 소켓 버퍼에 보류 중인 데이터가 있는 동안 EOF가 반환될 수 있다.
		- Vnodes
			- 파일 포인터가 파일의 끝이 아닌 경우 반환한다.
			- 데이터는 현재 위치에서 파일 끝까지의 오프셋을 포함하며 음수일 수 있다.
		- Fifos, Pipes
			- 읽을 데이터가 있을 때 반환한다. data는 사용 가능한 바이트 수가 저장된다.
			- 마지막 writer가 연결을 끊으면 filter가 EV_EOF를 설정한다. 이는 EV_CLEAR를 전달하여 지울 수 있다.
				- 이 시점에서 필터는 반환하기 전 데이터를 사용할 수 있을 때까지 대기한다.
		- Device nodes
			- device에서 읽을 데이터가 있는 경우 반환한다.
			- data는 사용 가능한 바이트 수가 저장된다.
				- device가 바이트 수 반환을 지원하지 않는 경우 사용을 허용하지 않는다.
- EVFILT_EXCEPT
	- fd에서 지정된 예외조건 중 하나가 발생할 때 반환.
	- 조건은 fflags에 지정된다. NOTE_OOB를 사용하여 소켓 fd에 대한 대역 외 데이터(out-of-band data)의 도착을 모니터링 하는 데 사용할 수 있다.
	- 소켓의 읽기 방향이 종료된 경우 필터는 flags에 EV_EOF를 설정하고 fflags에 소켓 오류를 반환(설정이 있는 경우)한다.
- EVFILT_WRITE
	- 파일 디스크립터를 식별자로 사용하고 디스크립터에 write할 수 있을 때 반환한다. 소켓 pipe 및 [[fifo]]의 경우 data에 쓰기 버퍼에 남아있는 공간이 포함된다. 
	- 이 필터는 reader의 연결 해제 시 EV_EOF를 설정한다.
		- fifo의 경우 EV_CLEAR를 통해 이를 지울 수 있다.
	- 이 필터는 vnode에 지원되지 않는다.
- EVFILT_AIO
	- 현재 지원되지 않음
- EVFILT_VNODE
	- 생략
- EVFILT_PROC
	- 모니터링할 PID를 식별자로 함. fflags를 감시할 이벤트로 사용하고 프로세스가 요청된 이벤트 중 하나 이상을 수행할 떄 반환한다. 프로세스가 일반적으로 다른 프로세스를 볼 수 있는 경우 이벤트를 연결할 수 있다.
		- NOTE_EXIT
			- 프로세스 종료
		- NOTE_EXITSTATUS
			- 프로세스 종료 시 data에 exit status 저장
			- NOTE_EXIT이 함께 설정되어 있을 때만 유효
		- NOTE_FORK
			- 프로세스가 fork()와 유사한 호출을 통해 자식 프로세스 생성
		- NOTE_EXEC
			- 프로세스가 execve()와 유사한 호출을 통해 새로운 프로새스를 실행
		- NOTE_SIGNAL
			- 프로세스에 시그널이 전송됨. waitpid()와 유사한 호출을 통해 상태를 확인할 수 있음.
- EVFILT_SIGNAL
	- 모니터링할 시그널 번호를 식별자로 사용하고, 지정된 시그널이 프로세스에 대해 생성될 때 반환.
	- 특정 스레드가 아닌 프ㅗ세스로 전송된 신호만 필터를 촉발한다.
	- 필터는 시그널이 SIG_IGN로 표시된 경우에도 프로세스에 시그널을 전달하려는 모든 시도를 기록
	- data는 kevent()에 대한 마지막 호출 이후 시그널이 생성된 횟수를 반환.
	- 내부적으로 EV_CLEAR를 자동으로 설정한다.
- EVFILT_TIMER
	- ident를 식별자로, data를 간격(밀리 초)의 타임아웃 인터벌 타이머를 설정한다.
	- 다른 타이머 유닛을 식별하기 위해 fflags는 다음 중 하나의 flag를 가질 수 있다.
		- NOTE_SECONDS 
			- data가 초 단위일 경우
		- NOTE_USECONDS 
			- data가 마이크로 초 단위일 경우
		- NOTE_NSECONDS 
			- data가 나노 초 단위일 경우
	- 타이머는 다른 타이머와 병합하여 power를 절약할 수 있다. 이 동작을 수행하기 위해 fflags에 다음 플래그를 설정할 수 있다.
		- NOTE_CRITICAL
			- 여유 값을 엄격하게 준수하기 위해 기본 절전 기술을 덮어쓴다.
		- NOTE_BACKGROUND
			- 다른 타이머와 병합하기 위해 더 많은 절전 기술을 적용한다.
	- EV_ONESHOT이 지정되지 않는 한 타이머는 주기적으로 발생한다.
		- 반환 시 데이터에는 타이머 이벤트의 마지막 준비 또는 마지막 전달 이후 시간 초과가 만료된 횟수가 포함된다.
		- 이 필터는 자동으로 EV_CLEAR 플래그를 설정한다.
```c++
EV_SET(&key, ident, filter, flags, fflags, data, udata);

void changeEvent(int nIdent, int nFilter, int nFlags, void *pUdata)
{
	ChangeList[nChanges].ident = nIdent;
	ChangeList[nChanges].filter = nfilter;
	ChangeList[nChanges].flags = nFlags;
	ChangeList[nChanges].fflags = 0;
	ChangeList[nChanges].data = 0;
	ChangeList[nChanges].udata = pUdata;
	nChanges++;
}
```
---
### 생각 (파생된 질문/생각)
- kqueue에 주는 타이머와, 소켓에 주는 타이머는 어떤 것을 우선순위로 두는가? 두 타이머가 작동되는 layer가 다르다면 어떻게 상호작용 할 것인가?
### 출 처 
- https://sncap.tistory.com/226
- https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/kqueue.2.html
- https://hyeonski.tistory.com/10
### 연결 문서: {연결 이유}
- 