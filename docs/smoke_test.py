"""Bounded local smoke checks for the unchanged academic server.
Run only in an isolated local development environment: the server binds all interfaces.
"""
from pathlib import Path
import socket, subprocess, time, json
ROOT = Path(__file__).resolve().parents[1]
with socket.socket() as probe:
    probe.bind(('127.0.0.1', 0))
    port = probe.getsockname()[1]
proc = subprocess.Popen([str(ROOT/'build/server'),str(port)],stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
connections=[]
def connect():
    for _ in range(50):
        try:
            s=socket.create_connection(('127.0.0.1',port),timeout=.3)
            s.settimeout(2);connections.append(s)
            return s,s.makefile('rb')
        except (ConnectionRefusedError,TimeoutError):time.sleep(.02)
    raise RuntimeError('Server did not start')
def cmd(s,f,text):
    s.sendall((text+'\n').encode());return f.readline().decode().rstrip('\r\n')
results=[]
def check(name,actual,expected):
    assert actual==expected,(name,actual,expected)
    results.append({'check':name,'result':'PASS'})
try:
    a,af=connect(); b,bf=connect()
    check('command before HELLO',cmd(a,af,'BORROW 1'),'ERR STATE not_authenticated')
    check('invalid username',cmd(a,af,'HELLO alice!'),'ERR PROTOCOL invalid_username')
    check('session A identity',cmd(a,af,'HELLO alice'),'OK HELLO alice')
    check('session B identity',cmd(b,bf,'HELLO bob'),'OK HELLO bob')
    check('list header',cmd(a,af,'LIST'),'LIST 15')
    items=[af.readline().decode().strip() for _ in range(15)]
    assert items[0]=='1 Camera FREE' and items[-1]=='15 VR_Headset FREE'
    results.append({'check':'initial inventory entries','result':'PASS'})
    check('borrow',cmd(a,af,'BORROW 1'),'OK BORROWED 1')
    check('competing borrow',cmd(b,bf,'BORROW 1'),'ERR UNAVAILABLE borrowed_by=alice')
    check('wrong-owner return',cmd(b,bf,'RETURN 1'),'ERR PERMISSION not_owner')
    check('wait on own loan',cmd(a,af,'WAIT 1'),'ERR DEADLOCK item')
    b.sendall(b'WAIT 1\n')
    check('owner return',cmd(a,af,'RETURN 1'),'OK RETURNED 1')
    check('wait notification',bf.readline().decode().strip(),'OK AVAILABLE 1')
    check('unknown item',cmd(a,af,'BORROW 999'),'ERR NOT_FOUND item')
    check('invalid ID',cmd(a,af,'BORROW abc'),'ERR PROTOCOL invalid_id')
    check('unknown command',cmd(a,af,'BOGUS'),'ERR PROTOCOL invalid_command')
    check('quit A',cmd(a,af,'QUIT'),'OK BYE')
    check('quit B',cmd(b,bf,'QUIT'),'OK BYE')
    print(json.dumps({'checks':results,'scope':'Server protocol only; two local sockets, one run. Not a security audit or stress test.'},indent=2))
finally:
    for s in connections:s.close()
    proc.terminate()
    try:proc.wait(timeout=2)
    except subprocess.TimeoutExpired:proc.kill();proc.wait()
