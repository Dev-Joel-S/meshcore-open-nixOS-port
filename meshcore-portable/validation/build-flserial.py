#!/usr/bin/env python3
"""Maintainer rebuild of the pinned native plugin; no Flutter SDK needed.
Use the same Debian 13 toolchain/runtime base as provenance.json.
"""
from pathlib import Path
import argparse, subprocess, tarfile, tempfile, os
parser=argparse.ArgumentParser()
parser.add_argument('--output',default='libflserial.so')
args=parser.parse_args()
here=Path(__file__).resolve().parent
out=Path(args.output).resolve()
with tempfile.TemporaryDirectory(prefix='meshcore-flserial-') as tmp:
 work=Path(tmp)
 with tarfile.open(here/'flserial-source.tar.xz') as archive:
  archive.extractall(work,filter='data')
 subprocess.run(['patch','-p1','-i',str(here/'flserial-startup-race.patch')],cwd=work,check=True)
 src=work/'src'
 files=['native_libs/src/serial.cpp','native_libs/src/tinycthread.c',
        'native_libs/src/ringbuffer.c','flserial.cpp','native_libs/src/impl/unix.cpp',
        'native_libs/src/impl/list_ports/list_ports_linux.cpp']
 objects=[]
 for i,name in enumerate(files):
  obj=work/f'{i}.o';objects.append(str(obj));is_c=name.endswith('.c')
  cmd=[os.environ.get('CC','gcc') if is_c else os.environ.get('CXX','g++')]
  if not is_c:cmd+=['-std=c++11']
  cmd+=['-O2','-g','-fPIC','-fno-omit-frame-pointer','-D__STDC_NO_THREADS__=1',
        '-DDART_SHARED_LIB','-I'+str(src/'native_libs/include'),
        '-I'+str(src/'native_libs/include/serial'),'-c',str(src/name),'-o',str(obj)]
  subprocess.run(cmd,check=True)
 subprocess.run([os.environ.get('CXX','g++'),'-shared','-Wl,--build-id=sha1',
                 '-Wl,-soname,libflserial.so','-Wl,-rpath,$ORIGIN',*objects,
                 '-pthread','-lrt','-o',str(out)],check=True)
 subprocess.run(['strip','--strip-debug',str(out)],check=True)
 out.chmod(0o755)
 print(out)
