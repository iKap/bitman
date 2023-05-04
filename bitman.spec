Name:	bitman	
Version:	1.5
Release:	1%{?dist}
Summary:	BitMan(bm) - command line Bit Manipulation Tool

Group:		Applications/System
License:	GPL
URL:		https://github.com/iKap/bitman
Source0:	%{name}-%{version}.tar.gz

BuildRequires:	gcc
Requires:	gcc	

%description
BitMan - command line Bit Manipulation Tool

%prep
%setup -q

%build
make %{?_smp_mflags} \
	CFLAGS="%{optflags}" \
	LDFLAGS="%{optflags}"


%install
make install DESTDIR=%{buildroot}


%files
%defattr(-,root,root,-)
/usr/bin/bm
%doc



%changelog
* Sun May 01 2023 Igor Kaplinsky <igor.kaplinsky@gmail.com> - 1.0-1
- Initial package release
