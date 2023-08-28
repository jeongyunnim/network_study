---
종류: "문헌"
날짜: "2023-08-23"
인물_저자: "데이빗 고울리,브라이언 토티,마조리 세이어,세일루 레디, 안슈 아가왈"
태그: "프로그래밍,네트워크"
출처: "HTTP 완벽 가이드"
구분_장소_분류: ""
Alias: "" # 한글/영어
---

### 날짜: 2023-08-23 19:46
### 태그: #프로그래밍/네트워크

>[!메모]
> URL의 포맷과 인터넷 상에서 URL이 가리키는 리소스의 다양한 형식에 대해 상세히 다룬다. URL에서 더 진화한 지시자인 URN에 대한 내용도 다룬다.
> - URL 문법
> - URL 컴포넌트
> - URL 인코딩, 문자 규칙
> - 공통 URL scheme

### 학습 내용
---
## URL
- URI의 부분집합
- 사용자는 URL에 명시된 HTTP 및 다른 프로토콜을 통해 원하는 리소스에 접근할 수 있다.
- HTTP는 URI를 더 일반화된 개념의 리소스 식별자로 사용한다.
	- 하지만 대부분 URL을 가리킨다.
![[Pasted image 20230825113710.png]]
- \https://naver.com/index.html
	- https://
		- URL의 스킴, 웹 클라이언트가 어떻게 리소스에 접근하는지 알려준다.
	- \www.naver.com
		- 서버의 위치, 웹 클라이언트에게 리소스가 어디에 호스팅 되어있는지 알려준다.
	- /index.html
		- 리소스의 경로, 서버에게 요청하는 리소스가 무엇인지 알려준다.
- 기타 URL
	- 이메일 주소: \mailto:president\@whitehouse.gov
	- FTP: \ftp://ftp.lots-o-books.com/pub/complete-price-list.xls
	- rtsp: \rtsp://www.joes-hardware.com:554/interview/cto_video
[[1-2-1 URL 문법]]
[[1-2-2 스킴의 포맷]]

---
### 생각 (파생된 질문/생각)
- 
### 출 처
- [Windows 소켓 프로그래밍 입문에서 고성능 서버까지! ](https://www.inflearn.com/course/%EC%9C%88%EB%8F%84%EC%9A%B0-%EC%86%8C%EC%BC%93-%EC%9E%85%EB%AC%B8-%EA%B3%A0%EC%84%B1%EB%8A%A5-%EC%84%9C%EB%B2%84)

### 연결 문서: {연결 이유}
- 