#!/bin/bash
#INSTALL@ /usr/local/bin/gimp.youcam.credits
#INSTALLEDFROM verlaine:/home/ljm/src/gimp-plugin
curlout=/tmp/curlout.$$

keyfile=youcam.api.key
if [ ! -f $keyfile ] ; then
	keyfile=~/.youcam.api.key
fi
if [ ! -f $keyfile ] ; then
	keyfile=~/usr/local/etc/youcam.api.key
fi
if [ ! -f $keyfile ] ; then
	keyfile=~/etc/youcam.api.key
fi
if [ ! -f $keyfile ] ; then
	echo "Youcam-keys: No keyfile found"
	exit 1
fi

client_id=$(sed -n 's/API key: //p' $keyfile)
client_secret=$(sed -n 's/Secret key: //p' $keyfile)



# Save the client_secret (public key) to a PEM file
echo "-----BEGIN PUBLIC KEY-----" > client_secret.pem
echo "$client_secret" >> client_secret.pem
echo "-----END PUBLIC KEY-----"  >> client_secret.pem

# Get the current timestamp in milliseconds
timestamp=$(($(date +%s%N)/1000000))

# Prepare the data to be encrypted
data="client_id=$client_id&timestamp=$timestamp"

# Encrypt the data using the public key
id_token=$(echo -n "$data" | openssl rsautl -encrypt -inkey client_secret.pem -pubin | base64)
id_token=$(paste -s -d '' <<< "$id_token")

# Clean up the temporary PEM file
rm -f client_secret.pem

curl -o $curlout -s -X POST "https://yce-api-01.perfectcorp.com/s2s/v1.0/client/auth" \
  -H "Content-Type: application/json" \
  -d "{
    \"client_id\": \"${client_id}\",
    \"id_token\": \"${id_token}\"
  }" 

access_token=$(sed 's/.*access_token":"//; s/".*//' $curlout)

rm -f $curlout
response=$(curl -s -X GET "https://yce-api-01.perfectcorp.com/s2s/v1.0/client/credit" -H "Authorization: Bearer $access_token" -H "Content-Type: application/json")

echo -n "Youcam Credits: "
echo "$response" | jq '[.results[].amount] | add'
