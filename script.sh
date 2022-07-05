echo
echo "TIME Sequential 720p"
echo
for j in {1..10}
do
    ./build/VideoMotionDetect 0 0 Resources/Video720p.mp4
    done
done

echo
echo "AVERAGE Read frame 720p"
echo
for b in {1..10}
    do
        ./build/Statistics 0 0 0 Resources/Video720p.mp4
    done
done

echo
echo "AVERAGE PUSH 1 threads 720p"
echo
for a in {1..10}
    do
        ./build/Statistics 1 0 0 Resources/Video720p.mp4
    done
done

echo
echo "AVERAGE PUSH 10 threads 720p"
echo
for a in {1..10}
    do
        ./build/Statistics 10 0 0 Resources/Video720p.mp4
    done
done


echo
echo "AVERAGE PUSH 20 threads 720p"
echo
for a in {1..10}
    do
        ./build/Statistics 20 0 0 Resources/Video720p.mp4
    done
done

echo
echo "AVERAGE PUSH 32 threads 720p"
echo
for a in {1..10}
    do
        ./build/Statistics 32 0 0 Resources/Video720p.mp4
    done
done


    
    
    
