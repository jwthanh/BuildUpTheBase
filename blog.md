----

June 19th 2017

Today was the first day of full-time indie gamedev. Lot of time today was spent anxious about leaving work. Not in the traditional sense of making sure I had a secure future, but worried that some work was left over and I'd get an angry email. Hope that works itself out.

I spent most of the putzing around with Factorio and Surge, but I spent a few hours working on tuning the mainloop for my game. I know I shouldn't be optimizing this early, and my game isn't that slow already, but a feature I've got right now is, since it's an incremental, is that it works even when you're not playing.

Currently it does one huge loop for all the time you were away, meaning if you were away for 24 hours, it would just run the mainloop once with a deltatime of 24 hours. This works alright, but the problem is that since your harvesters generate resources once, then your salesmen sell, the salesmen only sell once since it looped once. What I mean is that if you were actually playing for 24 hours straight you'd be loop several times a second meaning your workers would be generating and selling over and over again, instead of once each.

Since there's a storage limit for how many resources you can store, the harvesters need to have the salesmen sell to give them space to keep harvesting. So I want to accurately loop about one second at a time for every second lost when you're returning to the game. I don't know that I'll be able to actually do that for any time longer than a day without totally boring the player with a loading screen.

I've been playing Bloon TD and it's got some typical mobile game stuff where you need to tap your harvester buildings every time you come back or else they fill up. Maybe I can explore something like that, or weaken the simulation like only simulating 1000 seconds at a time instead of 1, or even simulating 5 minutes and then extrapolating that over the total time away.

So if the player generated 50 grain and 100 coins in 5 minutes, and they were gone for 10 minutes, I'd just fake it and add 100 grain and 200 coins to their banks. Of course that wouldn't work well with the storage limits but if I got smarter about maybe sanity checking the numbers after like say an hour's worth of output, it could be cool. I'm not sure the player would even notice so this isn't too big of a priority but I would like my game to perform at a smooth 60fps on as many phones as possible.

Speaking of performance, on a well established base, the phone does 200 or so draws a frame, 70% of which are text draws. I've been thinking about moving to BMP fonts instead of TTF to see if that helps as much as I'm hoping.

The problem is that I've never turned a TTF font into a BMP one, and I'd have to be smarter in my font usage. Right now almost everything shares a single configuration for color, size and outlines, but since BMP fonts don't scale as well, I think I'd need to set up a few different ones. We'll see though, there's plent of time for that in the future.
