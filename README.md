# cryfs-gpg
mount a CryFS files system. Encrypt and decrypt the CryFS config and password files using PGP encryption (GnuPG). Both files are renamed to the CryFS file standard     * name format and are truncate to the same block size. So they  are disguised among the other CryFS files.  Password is generate using crypt SHA512 -  86 characters + salt (user name). The password file is encrypted using PGP public key.

Compilation:

$gcc -c $(ls *.c) && gcc $(ls *.o) -lcrypt -o cryfs-gpg
