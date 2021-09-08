
IF "%1"=="" GOTO NoParams

python GetJWT.py -p %1 -k %2


GOTO:EOF

:NoParams

python GetJWT.py -p pm-devices -k rsa_pm1_private.pem

