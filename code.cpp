#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cmath>
using namespace std;
double get_cosine_similarity(
    const unordered_map<string, double>& user1,
    const unordered_map<string, double>& user2)
{
    // user1 and user2 are maps that mapping item_id to rating and returning the cosine similarity between the two users
     
    //adding a new map to add common items that our users both rated
    unordered_map<string, double> common_rated_items;
    for (const auto& item : user1)
    {
        //checking if user2 rated for the item that user1 rated or not
        if (user2.count(item.first))
        {
            common_rated_items.emplace(item.first, 0);
        }
    }
    
    
    
    
    //firstly creating numerator, which is sum of productions for every rating that has the same index
    double numerator = 0;
    for (const auto& item : common_rated_items)
    {
        numerator += user1.at(item.first) * user2.at(item.first);
    }
    
    //creating first part of denumerator by adding squares of ratings for user 1
    double sumOfSquaresOfRatings_user1 = 0;
    for (const auto& item : user1)
    {
        sumOfSquaresOfRatings_user1 += pow(item.second, 2);
    }
    //creating second part of denumerator by adding squares of ratings for user 2
    double sumOfSquaresOfRatings_user2 = 0;
    for (const auto& item : user2)
    {
        sumOfSquaresOfRatings_user2 += pow(item.second, 2);
    }
    //creating denumerator by multipliying square roots of sum of squares of ratings for both users
    double denumerator = sqrt(sumOfSquaresOfRatings_user1) * sqrt(sumOfSquaresOfRatings_user2);
    if (denumerator == 0)
    {
        return 0;
    }
    return numerator / denumerator;
}
double get_pearson_correlation(
    const unordered_map<string, double>& user1,
    const unordered_map<string, double>& user2)
{
    // user1 and user2 are maps that mapping item_id to rating and returning the Pearson correlation between the two users

    //adding a new map to add common items that our users both rated
    unordered_map<string, double> common_rated_items;
    for (const auto& item : user1)
    {
        //checking if user2 rated for the item that user1 rated or not
        if (user2.count(item.first))
        {
            common_rated_items.emplace(item.first, 0);
        }
    }

    // calculating the mean ratings for each user
    double mean_user1 = 0;
    for (const auto& item : user1)
    {
        mean_user1 += item.second;
    }
    mean_user1 /= user1.size();

    double mean_user2 = 0;
    for (const auto& item : user2)
    {
        mean_user2 += item.second;
    }
    mean_user2 /= user2.size();

    // calculating the numerator of the Pearson correlation formula
    double numerator = 0;
    for (const auto& item : common_rated_items)
    {
        numerator += (user1.at(item.first) - mean_user1) * (user2.at(item.first) - mean_user2);
    }

    // calculating the sum of squares of ratings for user 1
    double sum_of_squares_user1 = 0;
    for (const auto& item : user1)
    {
        sum_of_squares_user1 += pow(item.second - mean_user1, 2);
    }

    // calculating the sum of squares of ratings for user 2
    double sum_of_squares_user2 = 0;
    for (const auto& item : user2)
    {
        sum_of_squares_user2 += pow(item.second - mean_user2, 2);
    }

    // calculating the denominator of the Pearson correlation formula
    double denominator = sqrt(sum_of_squares_user1) * sqrt(sum_of_squares_user2);

    if (denominator == 0)
    {
        return 0;
    }
    return numerator / denominator;
}
int main()
{
    //
    unordered_map<string, unordered_map<string, double>> user_item_ratings;

    // reading the train.csv file and create a map, mapping user_id to a map of item_id to rating
    
    ifstream train_file("train.csv");
    string line;
    getline(train_file, line); 
    while (getline(train_file, line))
    {
        stringstream lineStream(line);
        string user_id, item_id, rating_str;
        getline(lineStream, user_id, ',');
        getline(lineStream, item_id, ',');
        getline(lineStream, rating_str, ',');
        double rating = stod(rating_str);
        user_item_ratings[user_id][item_id] = rating;
    }

    // reading the test.csv file and for each (user_id, item_id) pair, compute the
    // cosine similarity between the user and all other users in train.csv who have rated the item
    
    ifstream test_file("test.csv");
    ofstream output_file("predictions.csv");
    output_file <<"ID"<<","<<"Predicted" << '\n';
    getline(test_file, line);
    int count = 0;
    

    
    while (getline(test_file, line))
    {
        
        std::cout << count<<endl;
        stringstream lineStream(line);
        string user_id, item_id;
        getline(lineStream, user_id, ',');
        getline(lineStream, item_id, ',');
        vector<pair<string, double>> similarities;
        //cout << "similarities for " << user_id << " who have voted for " << item_id << "\n";
        for (const auto& [other_user, ratings] : user_item_ratings)
        {
            if (ratings.count(item_id))
            {
                double similarity = get_pearson_correlation(user_item_ratings[user_id], ratings);
                similarities.emplace_back(other_user, similarity);
            }
        }
        sort(similarities.begin(), similarities.end(),
            [](const auto& firstPair, const auto& secondPair) { return firstPair.second > secondPair.second; });
        //cout << "Top 30 most similar users for " << user_id << ":\n";
        double predicted_rating = 0;
        
        int border;
        int top = 50;
        if (top > similarities.size()) {
            border = similarities.size();
        }
        else
            border = top;
            
        for (int i = 0; i < border; ++i)
        {
            const auto& [other_user, similarity] = similarities[i];
            double rating = user_item_ratings[other_user][item_id];
            predicted_rating += rating;
            //cout << "\t" << other_user << ": " << rating << "\n";
        }
        
        
        output_file <<count<<","<< predicted_rating / border << '\n';
        //cout << predicted_rating / border<<endl;
        count = count + 1;

    }
    output_file.close();
    std::cout << "final count" << count;
    return 0;
}
