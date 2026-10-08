// QR 메일 발송 (SMTP 설정은 .env)
const nodemailer = require('nodemailer');

const isConfigured = () => Boolean(process.env.SMTP_HOST);

const transport = isConfigured()
  ? nodemailer.createTransport({
      host: process.env.SMTP_HOST,
      port: Number(process.env.SMTP_PORT || 587),
      secure: process.env.SMTP_SECURE === 'true', // 465 면 true, 587 이면 false (STARTTLS)
      auth: process.env.SMTP_USER
        ? { user: process.env.SMTP_USER, pass: process.env.SMTP_PASS }
        : undefined,
    })
  : null;

// 서버 시작 시 SMTP 접속/인증 확인
async function verifySmtp() {
  if (!transport) return { ok: false, error: 'SMTP_HOST 미설정' };
  try {
    await transport.verify();
    return { ok: true };
  } catch (e) {
    return { ok: false, error: e.message };
  }
}

async function sendQrMail({ to, company, qrPng, accountLabel }) {
  if (!transport) throw new Error('SMTP 설정이 없습니다. .env 파일을 확인하세요.');
  await transport.sendMail({
    from: process.env.MAIL_FROM || process.env.SMTP_USER,
    to,
    subject: `[${company}] Microsoft Authenticator 등록 QR 코드`,
    html: `
      <p>안녕하세요.</p>
      <p><b>${escape(company)}</b> 로그인용 OTP 등록 QR 코드입니다.</p>
      <ol>
        <li>휴대폰에서 <b>Microsoft Authenticator</b> 앱을 엽니다.</li>
        <li><b>+</b> → <b>기타 계정(Google, Facebook 등)</b> 선택</li>
        <li>아래 QR 코드를 스캔합니다.</li>
      </ol>
      <p><img src="cid:otp-qr" alt="QR Code" width="240" height="240"></p>
      <p>등록 후 앱에 <b>${escape(company)} / ${escape(accountLabel)}</b> 로 표시되는 6자리 코드로 로그인하세요.</p>
      <p style="color:#b42318">이전에 같은 이름으로 등록한 항목이 앱에 있다면 더 이상 사용할 수 없으니 삭제하세요.
      (괄호 안의 날짜·시각이 가장 최근인 항목만 유효합니다)</p>
      <p style="color:#888;font-size:12px">본 메일은 타인에게 전달하지 마세요.</p>`,
    attachments: [{ filename: 'otp-qr.png', content: qrPng, cid: 'otp-qr' }],
  });
}

function escape(s) {
  return String(s).replace(/[&<>"']/g, (c) => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' }[c]));
}

module.exports = { sendQrMail, verifySmtp, isConfigured, escape };
