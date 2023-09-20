---
종류: 문헌
날짜: 2023-09-20
인물: mozilla
태그: 프로그래밍,네트워크
출처: https://developer.mozilla.org/
aliases:
---

### 날짜: 2023-09-20

### 태그: #프로그래밍/네트워크

>[!메모]
> 

### 학습 내용
---
## POST 메서드
- 서버로 데이터를 전송한다.
- 요청 본문의 유형은 **Content-Type 헤더**로 나타냄.
	- [[PUT]]과 POST의 차이는 멱등성의 유무이다.
- 일반적으로 HTML 양식을 통해 전송한다.
	- 이 데이터로 서버에 변경사항을 만든다.
	- \<form> 요소의 [[enctype]]특성이나 [`<input>` (en-US)](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input "Currently only available in English (US)"), [`<button>`](https://developer.mozilla.org/ko/docs/Web/HTML/Element/button)요소의 [`formenctype` (en-US)](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input "Currently only available in English (US)") 특성 안에 적당한 문자열을 넣어 결정합니다.
```HTML
<form action="" method="get" class="form-example">
  <div class="form-example">
    <label for="name">Enter your name: </label>
    <input type="text" name="name" id="name" required />
  </div>
  <div class="form-example">
    <label for="email">Enter your email: </label>
    <input type="email" name="email" id="email" required />
  </div>
  <div class="form-example">
    <input type="submit" value="Subscribe!" />
  </div>
</form>
```
- POST 요청의 예제
	- 기존 리소스에 주석 달기
	- 게시판, 뉴스 그룹, 메일링 리스트, 이와 유사한 시스템에 글 올리기
	- 회원가입 모달로 새로운 사용자 추가하기
	- 양식 전송 결과 등 데이터 블록을 데이터 처리 프로세스에 보내기
	- 이어붙이기 연사을 통한 데이터베이스 확장
- **[[캐시 가능]]**: 신선도 정보 포함 시 가능하다.
---
### 생각 (파생된 질문/생각)
- 
### 출 처
- 

### 연결 문서: {연결 이유}
- 