if [ "$1" ]; then
	ip=$1
else
	echo "Usage: deploy.sh <robot ip>"
	exit 1
fi

ssh root@$ip 'chmod -x /etc/trik/trikGui.sh && killall trikGui'
rsync -avz -e ssh . root@$ip:/home/root/trik
scp trikGui.sh root@$ip:/etc/trik/trikGui.sh
ssh root@$ip 'chmod +x /etc/trik/trikGui.sh'
