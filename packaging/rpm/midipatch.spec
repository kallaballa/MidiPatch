#
# spec file for package midipatch (Version 0.1)
#
# Copyright 2009 SUSE LINUX Products GmbH, Nuernberg, Germany.
# Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org>
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# needsrootforbuild

Summary: MidiPatch is a scriptable, modular MIDI-synthesizer
Name: midipatch
Version: 0.1
Release: 1
License: AGPLv3
Group: Applications/Multimedia
URL: https://github.com/kallaballa/sndcut
Packager: Amir Hassan <amir@viel-zu.org>
%if %{defined suse_version}
BuildRequires: gcc-c++ pkg-config sqlite3-devel zlib-devel alsa-devel make
Requires: sqlite3 zlib alsa
%else
BuildRequires: gcc-c++ pkg-config libsq3-devel zlib-devel alsa-lib-devel make
Requires: sqlite-libs alsa-lib zlib
%endif
Source:       %{name}-%{version}.tar.bz2
BuildRoot:    %{_tmppath}/%{name}-%{version}-build

%description
MidiPatch is a scriptable modular MIDI-synthesizer. The scripting is done in lua and it basically exposes Tonic as literal as possible to the scripts. Additionally to the MIDI interface, there is a web interface with live editor, a visual rack generated from the code, an oscilloscope and more.
 
%global debug_package %{nil}

# extract source tar ball
%prep
%setup -q

%build
./build_third.sh
make

%install
make DESTDIR="%{buildroot}" PREFIX="%{_prefix}" install 

%post
 exit 0

%clean
# clean up the hard disc after build
#rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%doc README.md LICENSE

%{_bindir}/midipatch
/usr/lib64/libpatchscript.so
/usr/lib64/libtonic.so
