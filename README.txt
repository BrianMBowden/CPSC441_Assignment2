README FOR CRYPTO-TWEET
Brian Bowden
October 21, 2018
CPSC 441 || Tut 04

compilation:
    compilation MUST be done in each separate folder
    ie, tweetServer/ 
        tweetClient/
    makefile supplied

    >make

    will compile all files in folder

running:
    server side --
        >./tweetServer
    must be run before client side.
    folder MUST contain file called "dictionary.txt" even if is empty

    client side --
        >./tweetClient
    user will be prompted for input on success of connection to server
    follow steps from there

TCP vs UDP
    This felt like an obvious choice to pick TCP, 1) I have already been exposed to it 
    with the previous assignment and 2) UDP is lossy, so if I need a message to be sent, 
    UDP would not guarantee its arrival, which seems like a flaw for any kind messaging
    service. I think TCP is the right choice, it is guaranteed to arrive, and the work
    load that has to be done seems relatively small for any worry about time being 
    expensive.

hashbrown
    I really wanted to create a hashing function that would have a small amount of collisions
    and still have O(1) timing. The algorithm is a little complicated and drawn out, but I had 
    to split some of the concatenated strings in order to not be in the magnitudes of 2^(4*16) 
    integer values... I think the library does a good job of what is going on in the hash function

Bugs:
    I know for a fact that I did not handle every error I could think of (especially on the 
    server side), and I also did not handle punctuation in any tweet; I could think of ways 
    to do it, but time limiting, I opted not to in order to get other things working. Right 
    now, any punctuation is simply included in the encoding process.