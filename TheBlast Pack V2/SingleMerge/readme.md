# Single Merge V1.0 Stable

This is gonna be the thing that allows me to merge the .exe and the .zip data file.
It is actually working and the game already works with it.
I'm not doing a new repository just to this. I don't think it's a big deal.

# How does it work?

It opens the .exe as `ab+` and the zip as `rb`, then it merges the first with the string `%LSW_DATA` and the zip content, so it knows that after `%LSW_DATA` there is a zip content. On the game code you should not use like `std::string path = "%LSW_DATA";` because it will read the exactly line in the code and not the actual division. Do something like `std::string path = "%LSW_" + std::string("DATA");`
