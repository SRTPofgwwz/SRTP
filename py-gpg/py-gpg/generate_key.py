import gnupg

gpg = gnupg.GPG()

# generate key
input_data = gpg.gen_key_input(
    name_email='me@email.com',
    passphrase='passphrase',
)
key = gpg.gen_key(input_data)
print(key)

# create ascii-readable versions of pub / private keys
ascii_armored_public_keys = gpg.export_keys(key.fingerprint)
ascii_armored_private_keys = gpg.export_keys(
    keyids=key.fingerprint,
    secret=True,
    passphrase='passphrase',
)

# export
with open('mykeyfile.asc', 'w') as f:
    f.write(ascii_armored_public_keys)
    f.write(ascii_armored_private_keys)
