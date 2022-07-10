echo
echo "COMPLETION TIME SENZA ONDEMAND"
echo
for i in {1..32}
do
echo
echo "Esecuzione con $i threads"
echo
    for j in {1..10}
    do
        ./build/VideoMotionDetect $i 1 0 Resources/Video720p.mp4 
    done       
done

echo
echo "COMPLETION TIME CON ONDEMAND"
echo
for a in {1..32}
do
echo
echo "Esecuzione con $a threads"
echo
    for b in {1..10}
    do
        ./build/VideoMotionDetect $a 1 1 Resources/Video720p.mp4 
    done       
done
    
    
