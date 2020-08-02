# install:
# pip3 install python-gnupg

import gnupg

gpg = gnupg.GPG()

# import
with open('mykeyfile.asc') as f:
    key_data = f.read()
import_result = gpg.import_keys(key_data)


# for k in import_result.results:
#     print(k)

# encrypt file
with open('plain.txt', 'rb') as f:
    status = gpg.encrypt_file(
        file=f,
        recipients=['me@email.com'],
        output='encrypted.gpg',
    )

print(status.ok)
print(status.status)
print(status.stderr)
print('~'*50)