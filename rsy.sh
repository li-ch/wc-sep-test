make clean
make test
rsync test 172.16.18.205::upload
rsync test 172.16.18.198::upload
rsync test 172.16.18.207::upload
echo "rsync done"
