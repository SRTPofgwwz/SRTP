import gnupg
gpg = gnupg.GPG()
# import
def decryptor(file_name, keyfile_name, output_name):
    with open(keyfile_name) as f:
        key_data = f.read()
    import_result = gpg.import_keys(key_data)

    for k in import_result.results:
        print(k)

    # decrypt file
    with open(file_name, 'rb') as f:
        status = gpg.decrypt_file(
            file=f,
            passphrase='passphrase',
            output=output_name,
        )

    print(status.ok)
    print(status.status)
    print(status.stderr)
