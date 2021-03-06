# cryfs-gpg
mount a CryFS files system. Encrypt and decrypt the CryFS config and password files using PGP encryption (GnuPG). Both files are renamed to the CryFS file standard     * name format and are truncate to the same block size. So they  are disguised among the other CryFS files.  Password is generate using crypt SHA512 -  86 characters + salt (user name). The password file is encrypted using PGP public key.

Install cryfs

Manjaro:

sudo pacman -S cryfs 

Ubuntu/Debian:

sudo apt install cryfs

Other: 

https://github.com/cryfs/cryfs

cryfs-gpg compilation:

$git clone https://github.com/ddomingues1970/cryfs-gpg.git

$cd cryfs-gpg

$gcc -c $(ls *.c) && gcc $(ls *.o) -lcrypt -o cryfs-gpg

Example - Create new cryfs-gpg encrypted files:

$mkdir -p ~/dropbox/basedir ~/dropbox/mountdir

$cryfs-gpg -b ~/dropbox/basedir -m ~/dropbox/mountdir -r yourpgpkeymail@gmail.com 

Copy your files to ~/dropbox/mountdir 

Now you can use rclone to sync the encrypted files located in ~/dropbox/basedir to dropbox or any other 
cloud storage service (mega, onedrive, etc.).

See: https://rclone.org/

After unmounting ~/dropbox/mountdir using cryfs-unmount ~/dropbox/mountdir

To mount the cryfs-gpg execute the command:

$cryfs-gpg -b ~/dropbox/basedir -m ~/dropbox/mountdir

No need to remember cryfs password or config file location. :-)
Both are encrypted using your PGP public key. (-r parameter) and decrypted with private key.

The password generated for cryfs looks like this:
 
'$user1$fjNKZdaevYT94d.lu3gXRf7SNHcu0HZGzwNMpxe5zDtL4kj.JsXIe.1bMkW0yGrM6gooErJOkuYo0yyuqpbqz1'

It has 94 characters and virtually impossible to crack.

It would take a computer 71,141,121,218,454 quinquagintillion years to crack that password according 
https://www.comparitech.com/privacy-security-tools/password-strength-test/

This simple program is just a proof of concept, the correct way would be fork cryfs from github and implement this functionality. 

Privacy: Nothing to hide, everthing to protect. 

