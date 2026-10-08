const crypto = require('crypto');
const fs = require('fs');
const path = require('path');
const util = require('util');

// --log <파일> 이면 콘솔 출력을 시각과 함께 파일에 기록 (작업 스케줄러 실행용)
const logArg = process.argv.indexOf('--log');
if (logArg > 0 && process.argv[logArg + 1]) {
  const logFile = path.resolve(process.argv[logArg + 1]);
  fs.mkdirSync(path.dirname(logFile), { recursive: true });
  const write = (...a) =>
    fs.appendFileSync(logFile, `[${new Date().toLocaleString('sv-SE')}] ${util.format(...a)}\n`);
  console.log = console.warn = console.error = write;
  process.on('uncaughtException', (e) => {
    write('[FATAL]', e);
    process.exit(1);
  });
}

// 등록 시각 표시용 "MM/DD HH:mm"
const stamp = (d = new Date()) => {
  const p = (n) => String(n).padStart(2, '0');
  return `${p(d.getMonth() + 1)}/${p(d.getDate())} ${p(d.getHours())}:${p(d.getMinutes())}`;
};
const express = require('express');
const session = require('express-session');
const QRCode = require('qrcode');
const { authenticator } = require('otplib');

const store = require('./store');
const views = require('./views');
const { sendQrMail, verifySmtp } = require('./mailer');

const PORT = Number(process.env.PORT || 3000);
// 인증 성공 후 보여줄 페이지 (기본: tools/dcs_uart_viewer.html)
const VIEWER_FILE = path.resolve(process.env.VIEWER_FILE || path.join(__dirname, '..', '..', 'dcs_uart_viewer.html'));
const ISSUER_SUFFIX = process.env.OTP_ISSUER || ''; // 예: "MyService" → "회사명 (MyService)"
const MAX_ATTEMPTS = 5;
const LOCK_MINUTES = 5;

// TOTP: 30초, 6자리, SHA1 (MS Authenticator 기본값). 앞뒤 1스텝(±30초) 시계 오차 허용.
authenticator.options = { step: 30, digits: 6, window: 1 };

const app = express();
app.disable('x-powered-by');
app.use(express.urlencoded({ extended: false }));
app.use(
  session({
    secret: process.env.SESSION_SECRET || crypto.randomBytes(32).toString('hex'),
    resave: false,
    saveUninitialized: false,
    cookie: { httpOnly: true, sameSite: 'lax', maxAge: 60 * 60 * 1000 },
  })
);

const takeFlash = (req) => {
  const f = req.session.flash;
  delete req.session.flash;
  return f || {};
};
const clean = (s) => String(s || '').trim();
const EMAIL_RE = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;

// 새 TOTP 시크릿 생성 → QR 생성 → 메일 전송. 성공 시 secret 반환 (실패 시 throw)
// 앱 목록에서 재등록 전/후 항목을 구분할 수 있도록 계정명에 등록 시각을 붙인다.
async function issueQr(company, email) {
  const secret = authenticator.generateSecret(20); // 160bit
  const issuer = ISSUER_SUFFIX ? `${company} (${ISSUER_SUFFIX})` : company;
  const accountLabel = `${email} (${stamp()})`;
  const otpauthUrl = authenticator.keyuri(accountLabel, issuer, secret);
  const qrPng = await QRCode.toBuffer(otpauthUrl, { width: 300, margin: 2 });
  await sendQrMail({ to: email, company, qrPng, accountLabel });
  return secret;
}

// OTP 검증 (계정 잠금 + 재사용 방지). record 는 { secret, lastUsedStep, failedCount, lockedUntil }
// save(patch) 로 상태를 저장. 반환: { ok: true } 또는 { ok: false, text }
function checkOtp(record, code, save) {
  if (record && record.lockedUntil && Date.parse(record.lockedUntil) > Date.now()) {
    return { ok: false, locked: true, text: '계정이 일시적으로 잠겼습니다. 잠시 후 다시 시도하세요.' };
  }
  if (!record || !/^\d{6}$/.test(code)) return { ok: false, text: 'OTP 코드가 올바르지 않습니다.' };

  const delta = authenticator.checkDelta(code, record.secret);
  if (delta === null) {
    const failed = (record.failedCount || 0) + 1;
    save({
      failedCount: failed >= MAX_ATTEMPTS ? 0 : failed,
      lockedUntil: failed >= MAX_ATTEMPTS ? new Date(Date.now() + LOCK_MINUTES * 60000).toISOString() : null,
    });
    return { ok: false, text: 'OTP 코드가 올바르지 않습니다.' };
  }

  const usedStep = Math.floor(Date.now() / 30000) + delta;
  if (record.lastUsedStep != null && usedStep <= record.lastUsedStep) {
    return { ok: false, text: '이미 사용된 코드입니다. 다음 코드를 기다려 주세요.' };
  }
  save({ lastUsedStep: usedStep, failedCount: 0, lockedUntil: null });
  return { ok: true };
}

// ───────────── 1. 관리자 페이지 ─────────────
// 1.1 회사이름 + EMAIL → TOTP 시크릿 생성 → QR 생성 → 이메일 전송
// 이미 등록된 정보면 재등록 여부를 먼저 묻는다 (confirm=yes 로 다시 들어오면 재발송)
app.get('/admin', (req, res) => res.send(views.admin(takeFlash(req).msg)));

app.post('/admin/register', async (req, res) => {
  const company = clean(req.body.company);
  const email = clean(req.body.email);
  if (!company || !EMAIL_RE.test(email)) {
    req.session.flash = { msg: { type: 'err', text: '회사 이름과 올바른 EMAIL 을 입력하세요.' } };
    return res.redirect('/admin');
  }
  const exists = Boolean(store.getUser(company, email));
  if (exists && req.body.confirm !== 'yes') {
    console.log(`[등록] 이미 등록됨 → 재등록 확인 표시: ${company} / ${email}`);
    return res.send(views.adminConfirm(company, email));
  }
  try {
    const secret = await issueQr(company, email);
    store.upsertUser(company, email, secret); // 메일 발송 성공 후에만 저장 (재등록 시 기존 시크릿 교체)
    console.log(`[등록] ${exists ? '재등록(새 비밀키)' : '신규 등록'} QR 메일 전송: ${company} / ${email}`);
    res.send(views.adminSent());
  } catch (e) {
    console.error('메일 발송 실패:', e);
    req.session.flash = { msg: { type: 'err', text: `메일 발송 실패: ${e.message}` } };
    res.redirect('/admin');
  }
});

app.get('/admin/cancel', (req, res) => {
  console.log(`[등록] 재등록 취소(비밀키 유지): ${clean(req.query.company)} / ${clean(req.query.email)}`);
  res.send(views.adminCancel());
});

// ───────────── 3. 사용자 페이지 ─────────────
app.get('/', (req, res) => res.redirect(req.session.user ? '/viewer' : '/login'));

app.get('/login', (req, res) => res.send(views.login(takeFlash(req).msg)));

// 3.1 회사이름 + EMAIL → OTP 입력 페이지로 이동
// 등록 여부와 관계없이 항상 /verify 로 보냄 (계정 존재 여부 노출 방지)
app.post('/login', (req, res) => {
  const company = clean(req.body.company);
  const email = clean(req.body.email);
  if (!company || !email) return res.redirect('/login');
  req.session.regenerate(() => {
    req.session.pending = { company, email, attempts: 0 };
    res.redirect('/verify');
  });
});

app.get('/verify', (req, res) => {
  if (!req.session.pending) return res.redirect('/login');
  res.send(views.verify(req.session.pending.email, takeFlash(req).msg));
});

// 4. OTP 검증 → WELCOME
app.post('/verify', (req, res) => {
  const pending = req.session.pending;
  if (!pending) return res.redirect('/login');

  const user = store.getUser(pending.company, pending.email);
  const result = checkOtp(user, clean(req.body.code), (patch) =>
    store.updateUser(user.company, user.email, patch)
  );

  if (!result.ok) {
    console.log(`[로그인] 실패: ${pending.company} / ${pending.email}${user ? '' : ' (미등록)'} - ${result.text}`);
    if (result.locked) {
      req.session.flash = { msg: { type: 'err', text: result.text } };
      return res.redirect('/verify');
    }
    pending.attempts += 1;
    if (pending.attempts >= MAX_ATTEMPTS) {
      delete req.session.pending;
      req.session.flash = { msg: { type: 'err', text: '시도 횟수를 초과했습니다. 다시 로그인하세요.' } };
      return res.redirect('/login');
    }
    req.session.flash = { msg: { type: 'err', text: `${result.text} (${MAX_ATTEMPTS - pending.attempts}회 남음)` } };
    return res.redirect('/verify');
  }

  console.log(`[로그인] 성공: ${user.company} / ${user.email}`);
  req.session.regenerate(() => {
    req.session.user = { company: user.company, email: user.email };
    res.redirect('/viewer');
  });
});

// 인증 성공 후 DCS UART Viewer (로그인한 세션만 접근 가능)
app.get('/viewer', (req, res) => {
  if (!req.session.user) return res.redirect('/login');
  res.set('Cache-Control', 'no-store'); // 로그아웃 후 뒤로가기로 열리지 않게
  res.sendFile(VIEWER_FILE, (err) => {
    if (err && !res.headersSent) res.status(500).send(`뷰어 파일을 찾을 수 없습니다: ${VIEWER_FILE}`);
  });
});

app.get('/logout', (req, res) => req.session.destroy(() => res.redirect('/login')));

const server = app.listen(PORT, async () => {
  console.log(`사용자 페이지:  http://localhost:${PORT}/login`);
  console.log(`관리자 페이지:  http://localhost:${PORT}/admin`);
  const smtp = await verifySmtp();
  console.log(smtp.ok ? 'SMTP 연결 확인 OK' : `[WARN] SMTP 사용 불가: ${smtp.error} → .env 를 확인하세요.`);
});
server.on('error', (e) => {
  if (e.code === 'EADDRINUSE') {
    console.error(`[ERROR] 포트 ${PORT} 를 이미 다른 프로그램(다른 서버 실행 중?)이 사용 중입니다. 기존 서버를 종료하세요.`);
    process.exit(1);
  }
  throw e;
});
