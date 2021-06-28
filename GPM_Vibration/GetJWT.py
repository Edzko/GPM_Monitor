import sys, getopt
import datetime
import jwt

def create_jwt(project_id, private_key_file, algorithm):
    """Creates a JWT (https://jwt.io) to establish an MQTT connection.
    Args:
     project_id: The cloud project ID this device belongs to
     private_key_file: A path to a file containing either an RSA256 or
             ES256 private key.
     algorithm: The encryption algorithm to use. Either 'RS256' or 'ES256'
    Returns:
        A JWT generated from the given project_id and private key, which
        expires in 20 minutes. After 20 minutes, your client will be
        disconnected, and a new JWT will have to be generated.
    Raises:
        ValueError: If the private_key_file does not contain a known key.
    """

    token = {
        # The time that the token was issued at
        "iat": datetime.datetime.utcnow(),
        # The time the token expires.
        "exp": datetime.datetime.utcnow() + datetime.timedelta(minutes=60),
        # The audience field should always be set to the GCP project id.
        "aud": project_id,
    }

    #print(token)

    # Read the private key file.
    with open(private_key_file, "r") as f:
        private_key = f.read()

    print(
        "Creating JWT using {} from private key file {}".format(
            algorithm, private_key_file
        )
    )

    return jwt.encode(token, private_key, algorithm=algorithm)


def main(argv):
    projectname = ''
    keyfile = ''
    haveargs = 0
    try:
        opts, args = getopt.getopt(argv,"hp:k:",["project=","key="])
    except getopt.GetoptError:
        print('GetJWT.py -p <project name> -k <keyfile>')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print('GetJWT.py -p <project name> -k <keyfile>')
            sys.exit()
        elif opt in ("-p", "--project"):
            projectname = arg
            haveargs += 1
        elif opt in ("-k", "--key"):
            keyfile = arg
            haveargs += 1

    if haveargs<2:
        print('GetJWT.py -p <project name> -k <keyfile>')
        sys.exit()
        
    #mykey = create_jwt("my-project-1516154946730","c:/users/edzko/rsa_private.pem",algorithm="RS256")
    mykey = create_jwt(projectname,keyfile,algorithm="RS256")

    with open("JWT.tok", "w+") as f:
        f.write(mykey)

    print(mykey)
   
if __name__ == "__main__":
   main(sys.argv[1:])
