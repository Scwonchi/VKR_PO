#!/usr/bin/python3
import re
import datetime
import ldap
import ldap.modlist
import hashlib
from isc_dhcp_leases import Lease, IscDhcpLeases

current_date = datetime.datetime.now().strftime('%Y%m%d%H%M%SZ')
leases = IscDhcpLeases('/var/lib/dhcp/dhcpd.leases')
lease = {}
lease = leases.get()
n = 0
dl = len(lease)
ldap_con = ldap.initialize("ldap://10.1.1.200")
ldap_con.bind_s('dc=GOLOVA,dc=virtlabs', 'password')

def write_dhcp_lease_to_ldap_dhcp(lease_info):
	try:
	    dhcp_lease = {
		'objectClass': [b"top", b"dhcpLeases", b"domainRelatedObject", b"metaNIITP"],
		'cn': lease_info['hostname'],
		'dhcpHWAddress': lease_info['mac'],
		'dhcpAddressState': lease_info['ip'],
		'associatedDomain': lease_info['hostname'] + b'.virtlabs',
		'dhcpdManaged': lease_info['managed'],
		'dhcpExpirationTime': lease_info['end'],
		'dhcpdInstance': lease_info['instance']
	    }
	    ldif = ldap.modlist.addModlist(dhcp_lease)
	    ldap_con.add_s( 'cn=host' + lease_info['hostname'].decode('UTF-8') + ',cn=192.168.78.0,cn=config,dc=dhcp,dc=srv,dc=subj,dc=virtlabs', ldif)
	except ldap.LDAPError as error:
	     print('Error writing dhcp lease to ldap (dhcp) : {}'.format(error))
	     
	     
def write_dhcp_lease_to_ldap_dns(lease_info):
	try:
	    dns_lease = {
		'objectClass': [b"top", b"domain", b"domainRelatedObject", b"dNSDomain"],
		'aRecord': lease_info['ip'],
		'associatedDomain': b'host' + lease_info['hostname'] + b'.virtlabs'
	    }
	    ldif = ldap.modlist.addModlist(dns_lease)
	    ldap_con.add_s( 'dc=host' + lease_info['hostname'].decode('UTF-8') + ',dc=virtlabs,dc=dns,dc=srv,dc=subj,dc=virtlabs', ldif)
	except ldap.LDAPError as error:
	     print('Error writing dhcp lease to ldap (dns) : {}'.format(error))
	     
	     
	     
def write_dhcp_lease_to_ldap_dns_rev(lease_info):
	try:
	    dns_lease = {
		'objectClass': [b"top", b"domain", b"domainRelatedObject", b"dNSDomain", b"dNSDomain2"],
		'pTRRecord': b'host' + lease_info['hostname'] + b'.virtlabs',
		'associatedDomain': lease_info['h3'] + b'.' + lease_info['h2'] + b'.' + lease_info['h1'] + b'.' + lease_info['h0'] + b'.in-addr.arpa'
	    }
	    ldif = ldap.modlist.addModlist(dns_lease)
	    ldap_con.add_s( 'dc=' + lease_info['h3'].decode('UTF-8') + ',dc=' + lease_info['h2'].decode('UTF-8') + ',dc=' + lease_info['h1'].decode('UTF-8') + ',dc=' + lease_info['h0'].decode('UTF-8') + ',dc=in-addr,dc=arpa,dc=dns,dc=srv,dc=subj,dc=virtlabs', ldif)
	except ldap.LDAPError as error:
	     print('Error writing dhcp lease to ldap (dns_rev) : {}'.format(error))
	     
	     
def write_dhcp_lease_to_ldap_dns_rev0(lease_info):
	try:
	    dns_lease = {
		'objectClass': [b"top", b"domain"],
	    }
	    ldif = ldap.modlist.addModlist(dns_lease)
	    ldap_con.add_s( 'dc=' + lease_info['h0'].decode('UTF-8') + ',dc=in-addr,dc=arpa,dc=dns,dc=srv,dc=subj,dc=virtlabs', ldif)
	except ldap.LDAPError as error:
	     print('Error writing dhcp lease to ldap (dns_rev0) : {}'.format(error))
   
   
def write_dhcp_lease_to_ldap_dns_rev1(lease_info):
	try:
	    dns_lease = {
		'objectClass': [b"top", b"domain"],
	    }
	    ldif = ldap.modlist.addModlist(dns_lease)
	    ldap_con.add_s( 'dc=' + lease_info['h1'].decode('UTF-8') + ',' + 'dc=' + lease_info['h0'].decode('UTF-8') + ',dc=in-addr,dc=arpa,dc=dns,dc=srv,dc=subj,dc=virtlabs', ldif)
	except ldap.LDAPError as error:
	     print('Error writing dhcp lease to ldap (dns_rev1) : {}'.format(error))
   
   
def write_dhcp_lease_to_ldap_dns_rev2(lease_info):
	try:
	    dns_lease = {
		'objectClass': [b"top", b"domain"],
	    }
	    ldif = ldap.modlist.addModlist(dns_lease)
	    ldap_con.add_s( 'dc=' + lease_info['h2'].decode('UTF-8') + ',dc=' + lease_info['h1'].decode('UTF-8') + ',dc=' + lease_info['h0'].decode('UTF-8') + ',dc=in-addr,dc=arpa,dc=dns,dc=srv,dc=subj,dc=virtlabs', ldif)
	except ldap.LDAPError as error:
	     print('Error writing dhcp lease to ldap (dns_rev2) : {}'.format(error))
   
   
   
   
while n < dl:
	ip = lease[n].ip
	mac = lease[n].ethernet
	macs = mac.split(':')
	mac2 = macs[0] + macs[1] + macs[2] + macs[3]
	hashname = hashlib.shake_256(mac2.encode("utf-8")).hexdigest(length=3)
	print (hashname)
	hostname = ip.split('.')
	lease_info =  {
		'ip': str.encode(lease[n].ip),
		'mac': str.encode(lease[n].ethernet),
		'hostname': str.encode(hashname),
		'managed': str.encode("TRUE"),
		'end': (lease[n].end),
		'instance': str.encode("server1"),
		'h0': str.encode(hostname[0]),
		'h1': str.encode(hostname[1]),
		'h2': str.encode(hostname[2]),
		'h3': str.encode(hostname[3])
	}
	n = n + 1
	dt= lease_info['end']
	lease_info['end'] = str.encode(dt.strftime('%Y%m%d%H%M%SZ'))
	actual_lease = dt.strftime('%Y%m%d%H%M%SZ')
	if actual_lease > current_date:
	   print (lease_info['ip'], lease_info['mac'], lease_info['hostname'], lease_info['end'])
	   write_dhcp_lease_to_ldap_dhcp(lease_info)
	   write_dhcp_lease_to_ldap_dns(lease_info)
	   write_dhcp_lease_to_ldap_dns_rev0(lease_info)
	   write_dhcp_lease_to_ldap_dns_rev1(lease_info)
	   write_dhcp_lease_to_ldap_dns_rev2(lease_info)
	   write_dhcp_lease_to_ldap_dns_rev(lease_info)


filter_str = '(dhcpdManaged=TRUE)'
search_base = 'dc=dhcp,dc=srv,dc=subj,dc=virtlabs'
search_base2 = 'dc=dns,dc=srv,dc=subj,dc=virtlabs'
search_scope = ldap.SCOPE_SUBTREE
search_results = ldap_con.search_s(search_base, search_scope, filter_str, ['dhcpExpirationTime', 'dhcpAddressState'])
for dn, attrs in search_results:
	if 'dhcpExpirationTime' in attrs:
	    dhcpExpirationTime_value = attrs['dhcpExpirationTime'][0].decode('utf-8')
	    print(f"dhcpExpirationTime value for {dn}: {dhcpExpirationTime_value}")
	if dhcpExpirationTime_value < current_date:
	   del_ip  = attrs['dhcpAddressState'][0].decode('utf-8')
	   print("del_ip:", del_ip)
	   filter_str2 = '(aRecord=' + del_ip + ')'
	   search_results2 = ldap_con.search_s(search_base2, search_scope, filter_str2)
	   asd = del_ip.split('.')
	   asd_rev = asd[3] + '.' + asd[2] + '.' + asd[1] + '.' + asd[0]
	   filter_str3 = '(associatedDomain=' + asd_rev + '.in-addr.arpa)'
	   search_results3 = ldap_con.search_s(search_base2, search_scope, filter_str3)
	   ldap_con.delete_s(dn)
	   print("deleted", dn)
	   for dn, attrs in search_results2:
	   	ldap_con.delete_s(dn)
	   	print("deleted", dn)
	   for dn, attrs in search_results3:
	   	ldap_con.delete_s(dn)
	   	print("deleted", dn)
	   

ldap_con.unbind()
