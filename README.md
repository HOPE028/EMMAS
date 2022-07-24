# EMMAS

This was the custom script written for the active lower-body exoskleton.   
Project Link: https://projectboard.world/ysc/project/enhanced-mobility-mechanical-assistance-system-reducing-overexertion-injuries-in-the-manual-workforc


# Part 3:

## Quesion:

Currently, the endpoint that returns the onboarding form (GET /api/onboarding) returns hardcoded data. Can you describe the benefits/drawbacks around saving this onboarding steps data in the front-end, back-end (as hardcoded data), or in the database? When would you want to save the data at different levels of the stack?

## Answer:


Naturally, all three levels have unique benefits and drawbacks to saving the steps on to. 

Starting with the database, it is the most flexible, scalable and redundant. To change what the steps data contains, one must simply update the database without needing to change the code at any other level. This is great when it comes to data that is constantly being changed or will need to grow in size. It is also the most structured as all information can be saved in one place. Lastly, the database can store multiple copies of the same information in different locations, increasing security and the safety of the information. 
The drawbacks are primarily speed and money based. The time it takes to fetch data may increase loading times and if the database charges for every read and write, the constant need for data may result in expensive fees. 

Next is the back-end. The back-end is the hybrid between the database and front-end, providing the structure and flexibility (relative to the front-end) while maintaining minimal loading times and costs. In the actual case of the steps used on this project, the back-end is the most efficient place to store it. If there is ever a need to change the content, one can do so relatively easily while still getting great performance. With that said, the database ultimately has more redundancy and security than the back-end and can store greater amounts of information.

Lastly, we have the front-end. The front-end is generally limited to styling and displaying the text, but rarely stores the content itself. This is for several crucial reasons starting with the lack of flexibility. To change what the content says, we must rewrite the HTML files. If one needs to increase the number of steps that there are, then they must write all the content as well the code to display it. Storing information on the front-end may also present potential security risks if the information is sensitive, something that the other layers can much better protect against. Lastly, there is very little structure and is very hard to maintain as if a developer ever needs to find some specific content that must be changed, they must scan through A LOT of documents. 
The front-end does have one benefit, however, and that is that it is really fast. Since there is no fetching of data, everything is immediate. 

I hope I answered the questions in the wanted format. I also hope that the code was clean and that the comments helped make the code understandable. If there are any tips or suggestions that I should know when it comes to programming, please let me know as I love learning and improving the quality of my work. Lastly, thank you to whoever took the time to look at my work. I appreciate your consideration. 
