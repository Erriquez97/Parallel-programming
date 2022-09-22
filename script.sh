for i in {1..32}
do
echo
echo "Esecuzione con $i threads"
    for j in {1..10}
do
    echo
    ./build/VideoMotionDetect $i 1 0 Resources/Video720p.mp4 
done     
done   


    
    
