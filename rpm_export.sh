#!/bin/sh
##########################################
#
# Celestia Antwerp
# Project IBB MCU
#
# Copyright (c) Celestia Antwerp
#
# Created on: 27/10/2022
# By: Magdy Abdel
#
##########################################

PROJECT="ibb_mcu"
RPM_TOPDIR="${HOME}/rpm"
EXPORTDIR="/data/projects/ALL/RPMS/${PROJECT}"

echo "Exporting ${PROJECT} to ${EXPORTDIR}"
RPM=$(ls -1rt ${RPM_TOPDIR}/RPMS/x86_64/${PROJECT}-[0-9]*.[0-9]*.[0-9]*-*[0-9].x86_64.rpm | tail -1)
if [ -f ${RPM} ] ; then 
    if [ -d  ${EXPORTDIR} ] ; then 
        CMD="cp ${RPM} ${EXPORTDIR}"
        echo ${CMD}
        ${CMD}
        echo "Exported: ${EXPORTDIR}/$(basename ${RPM})"
    fi
else 
    echo "Nothing to release..."
    if [ -d  ${EXPORTDIR} ] ; then 
        if [ -f  ${EXPORTDIR}/$(basename ${RPM}) ] ; then 
            echo "${RPM} already available in ${EXPORTDIR}."
        else
            echo "${RPM}: Not found."
        fi
    fi
fi
