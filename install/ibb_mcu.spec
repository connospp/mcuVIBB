##########################################
#
# Celestia Antwerp
# Project IBB MCU
#
# Copyright (c) Celestia Antwerp
#
# Created on: 24/10/2022
# By: Magdy Abdel
#
##########################################

%define company_name    Celestia Antwerp
%define company_short   CAW
%define company_url     http://www.celestia-antwerp.be

%define project   ibb_mcu
%define summary   "IBB MCU"

%define rpm_name        %(echo %{project}|tr A-Z a-z)
%define rpm_packager    %(who am i | awk '{print $1}')

%define required_packages     python3
%define conflicting_packages  ""

## Version numbering
#
# proj_version   :  defines this release's version: MAJOR.MINOR.TEENY 
# proj_release   :  revision id in mercurial repository
# 
# Note:  Release RPMs are only generated when there are no outstanding 
#        changes in repository.
##
# version 01.01.02 --> 02.00.00 to allow new MCU interface
%define proj_version 02.00.00
%define hg_id  %(hg id -i)
%define hg_n   %(hg id -n)
%define hg_remote_id %(ssh hg@srvsmb001 "gethgid hg/%{project} %{hg_id}")
%define proj_release %(if [ -n "%{hg_remote_id}" ] ; then echo "%{hg_n}" ; else echo "65%{hg_n}unreleased" ; fi ;)
%define release_name %{proj_version}-%{proj_release}

# MCU bootloader repository and release tag
%define bootloader_repo ibb_mcu_bootloader
%define bootloader_tag  FWUPGR_RELEASE

%define mcu_image    IBBE.srec

# RPM Information
Summary:       %{summary}
Name:          %{rpm_name}
Version:       %{proj_version}
Release:       %{proj_release}
License:       %{company_name}
Group:         %{company_short}/%{rpm_name}
URL:           %{company_url}
Distribution:  %{company_short} %{project}
Vendor:        %{company_name}
%if %{required_packages} != ""
Requires:      %{required_packages}
%endif
%if %{conflicting_packages} != ""
Conflicts:     %{conflicting_packages}
%endif
BuildRoot:     %{_tmppath}/%{name}-root

# Build locations (local)
%define start_dir    %(pwd)
# Temporary directory e.g. hg clone
%define tmp_dir      %{start_dir}/tmp
# Install and file locations (target)
%define release_dir  /usr/local/%{company_short}/releases/%{project}/%{release_name}
%define project_dir  /usr/local/%{project}
%define bin_dir      /usr/local/bin

%description
%{summary}
=================================================================
Build by %{rpm_packager} on %(hostname) 
from %{start_dir}
Mercurial changeset: %(hg --debug id)
=================================================================
Provides : %{rpm_name}


###   #
# #  #   #####   #####   ######  #####
### #    #    #  #    #  #       #    #
   #     #    #  #    #  #####   #    #
  # ###  #####   #####   #       #####
 #  # #  #       #   #   #       #
#   ###  #       #    #  ######  #
%prep
set +x 

mkdir -p $RPM_BUILD_DIR/%{name}
# TODO Check build system (32/64bit, OS, ...)

###   #
# #  #   #####   #    #     #    #       #####
### #    #    #  #    #     #    #       #    #
   #     #####   #    #     #    #       #    #
  # ###  #    #  #    #     #    #       #    #
 #  # #  #    #  #    #     #    #       #    #
#   ###  #####    ####      #    ######  #####
%build
cd %{start_dir}

# Compile mcu code when possible in linux
mkdir -p build
cd build
rm -rf *
cmake ..
make
cd %{start_dir}
# TODO Replace tools by for loop

# TODO Build tools here instead of calling script
# # Build calibration tool
# $(cd tools/calibration ; ./build.sh)
# # Build configuration tool
# $(cd tools/configuration ; ./build.sh)

# Build bootloader firmware upgrade tool
mkdir -p %{tmp_dir}
cd %{tmp_dir}
if [ ! -d %{bootloader_repo} ] ; then
   hg clone ssh://hg@srvsmb001/hg/%{bootloader_repo}
fi
cd %{bootloader_repo}
hg pull
hg update %{bootloader_tag}
cd fwupgr
cmake .
make clean
make

# Go back to original directory
cd %{start_dir}

###   #
# #  #      #    #    #   ####    #####    ##    #       #
### #       #    ##   #  #          #     #  #   #       #
   #        #    # #  #   ####      #    #    #  #       #
  # ###     #    #  # #       #     #    ######  #       #
 #  # #     #    #   ##  #    #     #    #    #  #       #
#   ###     #    #    #   ####      #    #    #  ######  ######
%install
set +x 
cd %{start_dir}

# Create necessary directories
mkdir -p %{buildroot}%{release_dir}
mkdir -p %{buildroot}%{bin_dir}

# Copy MCU image to install location
if [ ! -f build/%{mcu_image} ] ; then
   echo "No MCU firmware image present in build directory. Exiting."
   exit -1
fi
cp build/%{mcu_image} %{buildroot}%{release_dir}/%{mcu_image}

# TODO Replace tools by for loop

# Create tools directory in install location
mkdir -p %{buildroot}%{release_dir}/tools

# Copy calibration tool to install location
cp tools/calibration/dist/mcu_calibration %{buildroot}%{release_dir}/tools/mcu_calibration
ln -s %{release_dir}/tools/mcu_calibration %{buildroot}%{bin_dir}/mcu_calibration

# Copy configuration tool to install location
cp tools/configuration/dist/mcu_configuration %{buildroot}%{release_dir}/tools/mcu_configuration
ln -s %{release_dir}/tools/mcu_configuration %{buildroot}%{bin_dir}/mcu_configuration

# Copy bootloader firmware upgrade tool to install location
cp %{tmp_dir}/%{bootloader_repo}/fwupgr/fwupgr %{buildroot}%{release_dir}/tools/fwupgr
ln -s %{release_dir}/tools/fwupgr %{buildroot}%{bin_dir}/fwupgr

# Copy upgrade tool wrapper to install location
cp tools/upgrade/mcu_upgrade.py %{buildroot}%{release_dir}/tools/mcu_upgrade.py
ln -s %{release_dir}/tools/mcu_upgrade.py %{buildroot}%{bin_dir}/mcu_upgrade

# Symbolic link to project directory
ln -s %{release_dir} %{buildroot}%{project_dir}

###   #
# #  #   ######     #    #       ######   ####
### #    #          #    #       #       #
   #     #####      #    #       #####    ####
  # ###  #          #    #       #            #
 #  # #  #          #    #       #       #    #
#   ###  #          #    ######  ######   ####
%files
%defattr(-,root,root)
%attr(555,root,root) %{release_dir}/[!i]*
%{project_dir}
%attr(-,root,root) %{bin_dir}


###   #
# #  #    ####   #       ######    ##    #    #
### #    #    #  #       #        #  #   ##   #
   #     #       #       #####   #    #  # #  #
  # ###  #       #       #       ######  #  # #
 #  # #  #    #  #       #       #    #  #   ##
#   ###   ####   ######  ######  #    #  #    #
%clean
set +x

echo "Cleaning up temporary directory %{tmp_dir}/%{bootloader_repo}"
rm -rf %{tmp_dir}/%{bootloader_repo}

echo "Cleaning up $RPM_BUILD_ROOT..."
chmod -R u+w $RPM_BUILD_ROOT/* 2>/dev/null
rm -rf $RPM_BUILD_ROOT/*
cd $RPM_BUILD_DIR/%{name}
[ -d $RPM_BUILD_DIR/%{name} ] && rm -rf $RPM_BUILD_DIR/%{name}
mkdir -p $RPM_BUILD_DIR/%{name}

###   #
# #  #   #####   #####   ######
### #    #    #  #    #  #
   #     #    #  #    #  #####
  # ###  #####   #####   #
 #  # #  #       #   #   #
#   ###  #       #    #  ######
%pre


###   #
# #  #   #####    ####    ####    #####
### #    #    #  #    #  #          #
   #     #    #  #    #   ####      #
  # ###  #####   #    #       #     #
 #  # #  #       #    #  #    #     #
#   ###  #        ####    ####      #
%post

###   #
# #  #   #####   #####   ######  #    #  #    #  
### #    #    #  #    #  #       #    #  ##   #  
   #     #    #  #    #  #####   #    #  # #  #  
  # ###  #####   #####   #       #    #  #  # #  
 #  # #  #       #   #   #       #    #  #   ##  
#   ###  #       #    #  ######   ####   #    #  
%preun


###   #
# #  #   #####    ####    ####    #####  #    #  #    #
### #    #    #  #    #  #          #    #    #  ##   #
   #     #    #  #    #   ####      #    #    #  # #  #
  # ###  #####   #    #       #     #    #    #  #  # #
 #  # #  #       #    #  #    #     #    #    #  #   ##
#   ###  #        ####    ####      #     ####   #    #
%postun

