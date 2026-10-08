// 사용자 저장소: data/users.json 단일 파일 (소규모 내부용).
// key = "회사명(소문자)|이메일(소문자)"
const fs = require('fs');
const path = require('path');

const DATA_DIR = process.env.DATA_DIR || path.join(__dirname, '..', 'data');
const FILE = path.join(DATA_DIR, 'users.json');

function load() {
  try {
    return JSON.parse(fs.readFileSync(FILE, 'utf8'));
  } catch {
    return {};
  }
}

function save(users) {
  fs.mkdirSync(DATA_DIR, { recursive: true });
  const tmp = FILE + '.tmp';
  fs.writeFileSync(tmp, JSON.stringify(users, null, 2));
  fs.renameSync(tmp, FILE);
}

const keyOf = (company, email) =>
  `${company.trim().toLowerCase()}|${email.trim().toLowerCase()}`;

function getUser(company, email) {
  return load()[keyOf(company, email)] || null;
}

function upsertUser(company, email, secret) {
  const users = load();
  users[keyOf(company, email)] = {
    company: company.trim(),
    email: email.trim(),
    secret,
    lastUsedStep: null,
    createdAt: new Date().toISOString(),
  };
  save(users);
}

function updateUser(company, email, patch) {
  const users = load();
  const k = keyOf(company, email);
  if (!users[k]) return;
  users[k] = { ...users[k], ...patch };
  save(users);
}

module.exports = { getUser, upsertUser, updateUser };
