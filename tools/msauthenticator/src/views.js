// 서버 렌더링 HTML 템플릿
const { escape } = require('./mailer');

const layout = (title, body) => `<!doctype html>
<html lang="ko"><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>${escape(title)}</title>
<style>
  *{box-sizing:border-box}
  body{margin:0;font-family:system-ui,-apple-system,"Malgun Gothic",sans-serif;background:#f3f5f9;color:#1d2433}
  .wrap{max-width:440px;margin:56px auto;padding:0 16px}
  .card{background:#fff;border-radius:12px;box-shadow:0 2px 12px rgba(0,0,0,.06);padding:28px}
  h1{font-size:20px;margin:0 0 20px}
  label{display:block;font-size:13px;color:#555;margin:14px 0 6px}
  input{width:100%;padding:11px 12px;border:1px solid #cfd6e2;border-radius:8px;font-size:15px}
  input.otp{font-size:28px;letter-spacing:12px;text-align:center;font-family:ui-monospace,monospace}
  button{margin-top:20px;width:100%;padding:12px;border:0;border-radius:8px;background:#0067b8;color:#fff;font-size:15px;cursor:pointer}
  .msg{padding:10px 12px;border-radius:8px;margin-bottom:14px;font-size:14px}
  .err{background:#fdecec;color:#b42318}.ok{background:#e8f5ec;color:#1b7a3d}
  .muted{color:#888;font-size:13px}
  a{color:#0067b8}
</style></head>
<body><div class="wrap">${body}</div></body></html>`;

const msgBox = (m) => (m ? `<div class="msg ${m.type}">${escape(m.text)}</div>` : '');

// 관리자: 회사이름 + EMAIL 입력 → QR 전송
exports.admin = (msg) => layout('QR 등록', `
<div class="card"><h1>MS Authenticator 등록 QR 발송</h1>${msgBox(msg)}
<form method="post" action="/admin/register">
  <label>회사 이름</label><input name="company" required maxlength="100" autofocus>
  <label>EMAIL</label><input type="email" name="email" required maxlength="200">
  <button>QR 생성 후 이메일 전송</button>
</form></div>`);

// 이미 등록된 회사이름 + EMAIL → 재등록 여부 확인
exports.adminConfirm = (company, email) => layout('재등록 확인', `
<div class="card"><h1>이미 등록되어 있습니다</h1>
<p><b>${escape(company)}</b><br>${escape(email)}</p>
<p>다시 등록하시겠어요?<br><span class="muted">
· <b>예</b>: 새 QR 코드가 전송되고, 앱에 등록된 기존 항목은 더 이상 사용할 수 없습니다. (새 QR 등록 후 기존 항목은 앱에서 삭제하세요)<br>
· <b>아니오</b>: 아무것도 바뀌지 않으며, 기존 항목을 그대로 사용합니다.</span></p>
<form method="post" action="/admin/register">
  <input type="hidden" name="company" value="${escape(company)}">
  <input type="hidden" name="email" value="${escape(email)}">
  <input type="hidden" name="confirm" value="yes">
  <button>예, 다시 등록합니다</button>
</form>
<form method="get" action="/admin/cancel">
  <input type="hidden" name="company" value="${escape(company)}">
  <input type="hidden" name="email" value="${escape(email)}">
  <button style="background:#888">아니오</button>
</form>
</div>`);

exports.adminCancel = () => layout('취소', `
<div class="card" style="text-align:center"><h1>페이지를 처음부터 다시 시작해 주세요</h1>
<p><a href="/admin">처음으로</a></p></div>`);

// 메일 전송 완료 화면 (여기서 끝)
exports.adminSent = () => layout('전송 완료', `
<div class="card" style="text-align:center"><h1>전송되었습니다</h1></div>`);

exports.login = (msg) => layout('로그인', `
<div class="card"><h1>로그인</h1>${msgBox(msg)}
<form method="post" action="/login">
  <label>회사 이름</label><input name="company" required autofocus>
  <label>EMAIL</label><input type="email" name="email" required>
  <button>다음</button>
</form></div>`);

exports.verify = (email, msg) => layout('OTP 인증', `
<div class="card"><h1>OTP 인증</h1>${msgBox(msg)}
<p class="muted">${escape(email)}<br>Microsoft Authenticator 앱에 표시된 6자리 코드를 입력하세요.</p>
<form method="post" action="/verify">
  <input class="otp" name="code" inputmode="numeric" pattern="[0-9]{6}" maxlength="6" autocomplete="one-time-code" required autofocus>
  <button>확인</button>
</form>
<p class="muted" style="margin-top:14px"><a href="/login">처음으로</a></p></div>`);
