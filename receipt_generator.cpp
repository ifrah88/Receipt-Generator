#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

class Item {
public:
    string itemName;
    string category;
    float price;
    int stock;
    int quantity;

    Item(string itemName, string category, float price, int stock, int quantity)
        : itemName(itemName), category(category), price(price), stock(stock), quantity(quantity) {}
};

class Node {
public:
    Item data;
    Node* next;

    Node(Item item, Node* next = nullptr) : data(item), next(next) {}
};

class Queue {
public:
    Node* front;
    Node* rear;

public:
    Queue() : front(nullptr), rear(nullptr) {}

    bool isEmpty()
    {
        return front == nullptr;
    }

    void enqueue(Item item) {
        Node* newNode = new Node(item);
        if (isEmpty()) {
            front = rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
    }

    Item dequeue() {
        if (isEmpty()) {
            cout << "Queue is empty" << endl;
            return Item("", "", 0, 0, 0); // Return a placeholder item for simplicity
        }

        Node* temp = front;
        Item item = front->data;
        front = front->next;

        if (front == nullptr) {
            rear = nullptr;
        }

        delete temp;
        return item;
    }

    void display() {
        Node* current = front;
        while (current != nullptr) {
            cout << "ITEM NAME: " <<"    "<<"CATEGORY: " <<"    "<<"PRICE: " <<"    "<<"STOCK: "<<"    "<<"QUANTITY: ";
            cout<<endl;
            cout <<current->data.itemName<<"            "<<current->data.category<<"      "<<current->data.price<<"      "<<current->data.stock<<"         "<<current->data.quantity;
            current = current->next;
            cout<<endl;
        }
    }

    // New method to calculate the size of the queue
    int size()  {
        int count = 0;
        Node* current = front;
        while (current != nullptr) {
            count++;
            current = current->next;
        }
        return count;
    }
};

class Stack {
private:
    Node* top;

public:
    Stack() : top(nullptr) {}

    bool isEmpty() const {
        return top == nullptr;
    }

    void push(Item item) {
        Node* newNode = new Node(item, top);
        top = newNode;
        printStackItems();
    }

    Item pop() {
        if (isEmpty()) {
            cerr << "Stack is empty" << endl;
            return Item("", "", 0, 0, 0); // Return a placeholder item for simplicity
        }

        Node* temp = top;
        Item item = top->data;
        top = top->next;

        delete temp;
        return item;
    }

    void display() {
        Node* current = top;
        while (current != nullptr) {
            cout << " Item Names: " << current->data.itemName << ", Category: " << current->data.category
                 << ", Price: " << current->data.price << ", Stock: " << current->data.stock
                 << ", Quantity: " << current->data.quantity << endl;
            current = current->next;
        }
    }

    // New function to print items in the stack
    void printStackItems() {
        cout << "Items in Stack:" << endl;
        cout<<endl;
        display();
        cout << endl;
    }
};

void displayReceipt(Queue cart, float totalPrice, string receiptCode) {
    cout << "Receipt Code: " << receiptCode << endl;
    cout << "---------------------------" << endl;
    Queue tempCart = cart; // Create a copy of the cart to display without modifying the original
    tempCart.display();
    cout << "---------------------------" << endl;
    cout << "Total Price: " << totalPrice << endl;
}

float calculateBill(Queue cart) {
    float totalBill = 0.0;
    Node* current = cart.front;
    while (current != nullptr) {
        totalBill += current->data.price * current->data.quantity; // Adjust total based on quantity
        current = current->next;
    }
    return totalBill;
}

// Recursive function to add items to the cart
void addItemToCart(Queue& cart, Stack& undoStack, float& totalPrice, const string& filename) {
    while (true) {
        cout << "Enter item name (or '0' to finish): ";
        string itemName;
        cin >> itemName;

        if (itemName == "0") {
            break; // User finished entering items
        }

        ifstream file(filename);
        string header;
        getline(file, header);

        Item newItem("", "", 0, 0, 0);
        bool itemFound = false;

        while (file.good()) {
            string line;
            getline(file, line);

            if (line.empty()) {
                continue; // Skip empty lines
            }

            replace(line.begin(), line.end(), ',', ' ');

            istringstream ss(line);
            string category, name;
            float price;
            int stock;

            ss >> name >> category >> price >> stock;

            if (name == itemName) {
                newItem = Item(name, category, price, stock, 0); // Quantity is initially 0
                itemFound = true;
                break;
            }
        }

        file.close();

        if (itemFound) {
            cout << "Enter quantity for " << itemName << ": ";
            int quantity;
            cin >> quantity;

            newItem.quantity = quantity;

            Stack cartState;
            while (!undoStack.isEmpty()) {
                cartState.push(undoStack.pop());
            }

            cart.enqueue(newItem);
            totalPrice += newItem.price * quantity;

            while (!cartState.isEmpty()) {
                undoStack.push(cartState.pop());
            }

            cout << "Item added to Bill." << endl;
        } else {
            cout << "Invalid item name. Please try again." << endl;
        }
    }

    // Check if '0' was entered to finish, otherwise recursively call the function for the next item
    if (cin.peek() == '\n') {
        cin.ignore(); // Consume the newline character
        return;
    } else {
        addItemToCart(cart, undoStack, totalPrice, filename);
    }
}
// Function to undo a specific item with a given quantity
void undoSpecificItem(Queue& cart, Stack& undoStack, float& totalPrice) {
    while (!cart.isEmpty()) {
        cout << "Enter the name of the item you want to undo (or 'no' to stop): ";
        string undoItemName;
        cin >> undoItemName;

        if (undoItemName == "no") {
            break;
        }

        Node* current = cart.front;
        Node* prev = nullptr;
        bool itemFound = false;

        while (current != nullptr) {
            if (current->data.itemName == undoItemName) {
                itemFound = true;
                break;
            }
            prev = current;
            current = current->next;
        }

        if (itemFound) {
            Item canceledItem = current->data;

            cout << "Do you want to undo the whole item or some of its quantity? (whole/quantity): ";
            string undoOption;
            cin >> undoOption;

            if (undoOption == "whole") {
                totalPrice -= canceledItem.price * canceledItem.quantity;
                cout << "The item has been fully removed from the cart." << endl;
            } else if (undoOption == "quantity") {
                cout << "Enter the quantity to undo for the item " << canceledItem.itemName << ": ";
                int undoQuantity;
                cin >> undoQuantity;

                if (undoQuantity >= canceledItem.quantity) {
                    totalPrice -= canceledItem.price * canceledItem.quantity;
                    cout << "The item has been fully removed from the cart." << endl;
                } else {
                    totalPrice -= canceledItem.price * undoQuantity;
                    canceledItem.quantity -= undoQuantity;
                    cart.enqueue(canceledItem);
                    cout << undoQuantity << " quantity of the item has been undone. Remaining quantity: "
                         << canceledItem.quantity << endl;
                }
            } else {
                cout << "Invalid option. Please enter 'whole' or 'quantity'." << endl;
                return;
            }

            if (prev == nullptr) {
                // If the first item is being removed
                cart.front = current->next;
            } else {
                prev->next = current->next;
            }

            delete current;

            undoStack.push(canceledItem);

            // Ask the user if they want to undo any other specific item
            cout << "Do you want to undo any other specific item? (yes/no): ";
            string undoOtherChoice;
            cin >> undoOtherChoice;

            if (undoOtherChoice == "no") {
                // Print the results if the user chooses not to undo any other specific item
                cout << "Results after undoing specific items:" << endl;
                cout << "Updated Cart:" << endl;
                cout << endl;
                cart.display();
                cout << endl;
                cout << "Bill for Updated Cart: " << calculateBill(cart) << endl;
                // Add any other relevant information or processing here
                break;
            }
        } else {
            cout << "Item not found in the cart. Please enter a valid item name or 'no' to stop." << endl;
        }
    }
}

float profit = 0.0;
int customerCount = 0;
int main() {
    int receiptNumber = 1111; // Starting receipt number

    while (true) {
        const string filename = "grocery_items.csv";

        ifstream file(filename);

        if (!file.is_open()) {
            cout << "Error opening file: " << filename << endl;
            return 1;
        }

        string header;
        getline(file, header);

        /*while (file.good()) {
            string line;
            getline(file, line);

            if (line.empty()) {
                continue;
            }

            replace(line.begin(), line.end(), ',', ' ');

            istringstream ss(line);
            string category, itemName;
            float price;
            int stock;

            ss >> itemName >> category >> price >> stock;

            cout << "Item Name " << itemName << ", Category: " << category
                 << ", Price: " << price << ", Stock: " << stock << endl;
        }

        file.close();*/

        Queue cart;
        Stack undoStack;
        float totalPrice = 0.0;
        int undoneItemsCount = 0;

        // Use the recursive function to add items to the cart
        addItemToCart(cart, undoStack, totalPrice, filename);

        // Simulate the user canceling items from the cart
        bool undoFirstItem = false;

        while (!cart.isEmpty()) {
            if (!undoFirstItem) {
                cout << endl;
                cout << "Do you want to undo the First item in the cart? (yes/no): ";
                string undoChoice;
                cin >> undoChoice;

                if (undoChoice == "yes") {
                    undoFirstItem = true;
                } else if (undoChoice == "no") {
                    break;
                } else {
                    cout << "Invalid choice. Please enter 'yes' or 'no'." << endl;
                }
            }

            if (undoFirstItem) {
                cout << endl;
                cout << "Do you want to undo the whole First item or some of its quantity? (whole/quantity): ";
                string undoOption;
                cin >> undoOption;

                if (undoOption == "whole") {
                    Item canceledItem = cart.dequeue();
                    totalPrice -= canceledItem.price * canceledItem.quantity;
                    undoneItemsCount += canceledItem.quantity;
                    cout << endl;
                    cout << "The First item has been fully removed from the cart." << endl;
                    undoStack.push(canceledItem);
                    undoFirstItem = false; // Ask to undo the First item again
                } else if (undoOption == "quantity") {
                    cout << endl;
                    cout << "Enter the quantity to undo for the First item: ";
                    int undoQuantity;
                    cin >> undoQuantity;

                    Item canceledItem = cart.dequeue();

                    if (undoQuantity >= canceledItem.quantity) {
                        totalPrice -= canceledItem.price * canceledItem.quantity;
                        undoneItemsCount += canceledItem.quantity;
                        cout << endl;
                        cout << "The First item has been fully removed from the cart." << endl;
                    } else {
                        totalPrice -= canceledItem.price * undoQuantity;
                        canceledItem.quantity -= undoQuantity;
                        cart.enqueue(canceledItem);
                        undoneItemsCount += undoQuantity;
                        cout << endl;
                        cout << undoQuantity << " quantity of the First item has been undone. Remaining quantity: "
                             << canceledItem.quantity << endl;

                        // Check if there is remaining quantity and ask the user if they want to delete it
                        if (canceledItem.quantity > 0) {
                            cout << "Do you want to delete the remaining quantity of the First item? (yes/no): ";
                            string deleteRemainingChoice;
                            cin >> deleteRemainingChoice;

                            if (deleteRemainingChoice == "yes") {
                                totalPrice -= canceledItem.price * canceledItem.quantity;
                                undoneItemsCount += canceledItem.quantity;
                                cout << "The remaining quantity of the First item has been fully removed from the cart." << endl;
                                // Note: You may want to add the remaining quantity to undoStack if needed.
                            }
                            // Print the results if the user chooses not to delete the remaining quantity
                            cout << "Results after undoing the First item:" << endl;
                            cout << "Updated Cart:" << endl;
                            cout << endl;
                            cart.display();
                            cout << endl;
                            cout << "Bill for Updated Cart: " << calculateBill(cart) << endl;
                            // Add any other relevant information or processing here
                        }
                    }
                    undoStack.push(canceledItem);
                    undoFirstItem = false; // Ask to undo the First item again
                } else {
                    cout << "Invalid option. Please enter 'whole' or 'quantity'." << endl;
                    // Reset the flag after undoing the First item
                    undoFirstItem = false;
                }
            }
        }

        // Check if all items were undone in undo First item
        if (cart.isEmpty() && undoneItemsCount > 0) {
            cout << "All items in the cart were undone. Cart is now empty." << endl;
        } else if (cart.size() == 1 && undoneItemsCount == 0) {
            // Directly ask about the quantity or whole of the single item in cart
            cout << "Do you want to undo the item? (yes/no): ";
            string undoSpecificChoice;
            cin >> undoSpecificChoice;

            if (undoSpecificChoice == "yes") {
                Item canceledItem = cart.dequeue();
                cout << "Do you want to undo the whole item or some of its quantity? (whole/quantity): ";
                string undoOption;
                cin >> undoOption;

                if (undoOption == "whole") {
                    totalPrice -= canceledItem.price * canceledItem.quantity;
                    undoneItemsCount += canceledItem.quantity;
                    cout << "The item has been fully removed from the cart." << endl;
                    undoStack.push(canceledItem);
                } else if (undoOption == "quantity") {
                    cout << "Enter the quantity to undo for the First item: ";
                    int undoQuantity;
                    cin >> undoQuantity;

                    // Check if entered quantity is valid
                    if (undoQuantity >= canceledItem.quantity) {
                        totalPrice -= canceledItem.price * canceledItem.quantity;
                        undoneItemsCount += canceledItem.quantity;
                        cout << "The item has been fully removed from the cart." << endl;
                    } else {
                        totalPrice -= canceledItem.price * undoQuantity;
                        canceledItem.quantity -= undoQuantity;
                        cart.enqueue(canceledItem);
                        undoneItemsCount += undoQuantity;
                        cout << undoQuantity << " quantity of the item has been undone. Remaining quantity: "
                             << canceledItem.quantity << endl;
                        undoStack.push(canceledItem);
                    }
                } else {
                    cout << "Invalid option. Please enter 'whole' or 'quantity'." << endl;
                }
            }
        } else if (cart.size() > 1) {
            // Ask the user if they want to undo any specific item only if there is more than one item in the cart
            cout << "Do you want to undo any specific item? (yes/no): ";
            string undoSpecificChoice;
            cin >> undoSpecificChoice;

            if (undoSpecificChoice == "yes") {
                undoSpecificItem(cart, undoStack, totalPrice);
            }
        }

        // Display the updated cart after the cancellation
        cout << "Updated Cart:" << endl;
        cout << endl;
        cart.display();

        // Check if all items were undone and print appropriate messages
        if (undoneItemsCount == 0 && cart.isEmpty()) {
            cout << "All items were undone by the user. Cart is now empty, and the total price is 0." << endl;
        }

        // Calculate and display the bill for the items in the updated cart
        float updatedCartBill = calculateBill(cart);
        cout << endl << "Bill for Updated Cart: " << updatedCartBill << endl;

        // Check if all items were undone and print appropriate messages
        if (undoneItemsCount == 0 && cart.isEmpty()) {
            cout << "All items were undone by the user. Cart is now empty, and the total price is 0." << endl;
        }

        // Calculate profit and update customer count
        profit += updatedCartBill; // Assuming profit is the total revenue from the items
        customerCount++;
        cout << endl;
        cout << "<---------------------------------------->";
        cout << endl << "RECEIPT FOR CUSTOMER : " << endl;
        cout << endl;
        // Display the receipt with the specific receipt number
        string receiptCode = "RC" + to_string(receiptNumber++);
        displayReceipt(cart, totalPrice, receiptCode);
        cout << endl;

        // Handle payment from the customer
        cout << "Enter the amount paid by the customer: ";
        float amountPaid;
        cin >> amountPaid;

        // Calculate the change to be given back to the customer
        float change = amountPaid - totalPrice;

        // Display the change
        cout << "Change to be given back: " << change << endl;
        // Ask the user if they want to shop again
        cout << "Do you want to Calculate Bill for another? (yes/no): ";
        string shopAgain;
        cin >> shopAgain;

        if (shopAgain != "yes") {
            cout << "Total Profit in 24 hours: " << profit << endl;
            cout << "Number of Customers in 24 hours: " << customerCount << endl;
            cout << "Thanks for shopping!" << endl;
            break;
        }
    }

    return 0;
}