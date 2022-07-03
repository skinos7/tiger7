#!/bin/sh

_dropbearkey()
{
	/usr/bin/dropbearkey "$@" 0<&- 1>&- 2>&-
}

# $1 - host key file name
hk_verify()
{
	[ -f "$1" ] || return 1
	[ -s "$1" ] || return 2
	_dropbearkey -y -f "$1" || return 3
	return 0
}

hk_generate_as_needed()
{
	local kdir kgen ktype tdir kcount tfile
	kdir='/etc/dropbear'

	kgen=''
	for ktype in ed25519 ecdsa rsa; do
		hk_verify "${kdir}/dropbear_${ktype}_host_key" && continue

		kgen="${kgen} ${ktype}"
	done

	[ -z "${kgen}" ] && return

	tdir=$(mktemp -d); chmod 0700 "${tdir}"

	kcount=0
	for ktype in ${kgen}; do
		tfile="${tdir}/dropbear_${ktype}_host_key"

		if ! _dropbearkey -t ${ktype} -f "${tfile}"; then
			# unsupported key type
			rm -f "${tfile}"
			continue
		fi

		kcount=$((kcount+1))
	done

	if [ ${kcount} -ne 0 ]; then
		mkdir -p "${kdir}"; chmod 0700 "${kdir}"; chown admin "${kdir}"
		mv -f "${tdir}/"* "${kdir}/"
	fi

	rm -rf "${tdir}"
}

hk_generate_as_needed

