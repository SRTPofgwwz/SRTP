import gnupg

gpg = gnupg.GPG()

# decrypt file
with open('encrypted.txt.gpg', 'rb') as f:
    status = gpg.decrypt_file(
        file=f,
        passphrase='passphrase',
        output='decrypted',
    )

print(status.ok)
print(status.status)
print(status.stderr)