const http = require('http');
const fs   = require('fs');
const path = require('path');

const PORT = 5000;
const DIR  = __dirname;

const MIME = {
    '.html': 'text/html',
    '.css':  'text/css',
    '.js':   'text/javascript',
};

function parseCSV(text) {
    const lines = text.trim().split('\n');
    const headers = lines[0].split(',');
    return lines.slice(1).map(line => {
        const vals = line.split(',');
        const obj = {};
        headers.forEach((h, i) => obj[h.trim()] = (vals[i] || '').trim());
        return obj;
    });
}

const server = http.createServer((req, res) => {
    const url = req.url.split('?')[0];

    // --- API routes ---
    if (url === '/api/logs') {
        try {
            const data = fs.readFileSync(path.join(DIR, 'log.csv'), 'utf8');
            res.writeHead(200, { 'Content-Type': 'application/json' });
            res.end(JSON.stringify(parseCSV(data)));
        } catch {
            res.writeHead(200, { 'Content-Type': 'application/json' });
            res.end('[]');
        }
        return;
    }

    if (req.method === 'DELETE' && url.startsWith('/api/logs/')) {
        const timestamp = decodeURIComponent(url.replace('/api/logs/', ''));
        try {
            const csvPath = path.join(DIR, 'log.csv');
            const text = fs.readFileSync(csvPath, 'utf8');
            const lines = text.trim().split('\n');
            const header = lines[0];
            const rows = lines.slice(1).filter(line => {
                const ts = line.split(',')[0].trim();
                return ts !== timestamp;
            });
            fs.writeFileSync(csvPath, [header, ...rows].join('\n') + '\n');
            res.writeHead(200, { 'Content-Type': 'application/json' });
            res.end('{"ok":true}');
        } catch {
            res.writeHead(500, { 'Content-Type': 'application/json' });
            res.end('{"ok":false}');
        }
        return;
    }

    if (url === '/api/users') {
        try {
            const data = fs.readFileSync(path.join(DIR, 'users.json'), 'utf8');
            res.writeHead(200, { 'Content-Type': 'application/json' });
            res.end(data);
        } catch {
            res.writeHead(200, { 'Content-Type': 'application/json' });
            res.end('{}');
        }
        return;
    }

    // --- Page routes ---
    const pageMap = { '/': 'home.html', '/users': 'users.html', '/tabel': 'tabel.html' };
    const fileName = pageMap[url] || url.slice(1);
    const filePath = path.join(DIR, fileName);
    const ext = path.extname(filePath);

    try {
        const content = fs.readFileSync(filePath);
        res.writeHead(200, { 'Content-Type': MIME[ext] || 'text/plain' });
        res.end(content);
    } catch {
        res.writeHead(404);
        res.end('Not found');
    }
});

server.listen(PORT, () => {
    console.log(`Server jalan di http://localhost:${PORT}`);
});
