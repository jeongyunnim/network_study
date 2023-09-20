---
종류: 문헌
날짜: 2023-09-20
인물: mozilla
태그: 프로그래밍
출처: https://developer.mozilla.org/
aliases:
---

### 날짜: 2023-09-20

### 태그: #프로그래밍/네트워크

>[!메모]
> 

### 학습 내용
---
## enctype
- method 특성이 post인 경우 enctype은 양식 제출 시 데이터의 [[MIME 타입 |MIME 유형]]을 나타낸다.
	- `application/x-www-form=urlencoded`: 기본값.
		- &으로 분리되고, "="으로 값과 키를 연결하는 map 형태로 인코딩된다. 이진 데이터는 이 플래그를 사용하기에 적합하지 않다. 따라서 mutlipart/form-data를 사용해야 한다.
	- `multipart/form-data`: \<input type="file">이 존재하는 경우 사용
	- `text/plain`: HTML5 에서 디버깅 용으로 추가된 값.
- [`<button>`](https://developer.mozilla.org/ko/docs/Web/HTML/Element/button), [`<input type="submit">` (en-US)](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input/submit "Currently only available in English (US)"), [`<input type="image">` (en-US)](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input/image "Currently only available in English (US)") 요소의 [`formenctype`](https://developer.mozilla.org/ko/docs/Web/HTML/Element/button#formenctype) 특성으로 재정의할 수 있다.

---
### 생각 (파생된 질문/생각)
- 
### 출 처
- https://developer.mozilla.org/ko/docs/Web/HTML/Element/form#enctype

### 연결 문서: {연결 이유}
- 