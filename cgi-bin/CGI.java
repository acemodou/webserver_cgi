import java.util.Arrays;

class sales_tax {
    public static void main(String[] args) {

        String[] lines = args[0].split("\n");

        if (lines[0].substring(0, 3).equals("GET")) {

            Double cost = Double.parseDouble(lines[0].split(" ")[1].split("\\?")[1].split("&")[0].split("=")[1]);
            Double rate = Double.parseDouble(lines[0].split(" ")[1].split("\\?")[1].split("&")[1].split("=")[1]);
            String result = Double.toString(cost * (rate / 100));
            System.out.println("HTTP/1.0 200 OK\nContent-type: text/html\n\n"
                    + "<html><head><title>SalesTax</title><meta charset=\"utf-8\">\n<link rel=\"stylesheet\" type=\"text/css\" href=\"../style.css\" />"
                    + "</head><body>" + "<div id=\"content\"><img src=\"../images/giphy.gif\">"
                    + "<h1>Sales Tax Calculator</h1>" + "<p>Enter the values below and click calculate.</p>\n"
                    + "<form action=\"../cgi-bin/sales_tax.class\" METHOD=GET>"
                    + "<div id=\"taxCalc\"><label>Item Cost: </label>"
                    + "<input type=\"text\" id=\"itemCost\" name=\"cost\"/><br>"
                    + "<label>Tax Rate :</label><input type=\"text\" id=\"taxRate\" name=\"rate\"/>%<br>"
                    + "<input type=\"submit\" value=\"Calculate\" /></div></form>" + "<div>" + result + "</div>"
                    + "<br><br><br>" + "<form ACTION=\"../cgi-bin/sales_tax.class\" METHOD=POST>"
                    + "<div id=\"taxCalc\">" + "<label>POST Item Cost: </label>"
                    + "<input type=\"text\" id=\"itemCost\" name=\"cost\" /><br>"

                    + "<label>POST Tax Rate :</label>"

                    + "<input type=\"text\" id=\"taxRate\" name=\"rate\" />%<br>"

                    + "<input type=\"submit\" value=\"Calculate\" /></div></form>" + "</body></html>");

        } else {

            Double cost = Double.parseDouble(lines[lines.length - 1].split("&")[0].split("=")[1]);
            Double rate = Double.parseDouble(lines[lines.length - 1].split("&")[1].split("=")[1]);
            String result = Double.toString(cost * (rate / 100));
            System.out.println("HTTP/1.0 200 OK\nContent-type: text/html\n\n"
                    + "<html><head><title>SalesTax</title><meta charset=\"utf-8\">\n<link rel=\"stylesheet\" type=\"text/css\" href=\"../style.css\" />"
                    + "</head><body>" + "<div id=\"content\"><img src=\"../images/giphy.gif\">"
                    + "<h1>Sales Tax Calculator</h1>" + "<p>Enter the values below and click calculate.</p>\n"
                    + "<form action=../cgi-bin/sales_tax.class METHOD=GET>"
                    + "<div id=\"taxCalc\"><label>Item Cost: </label>" + "<input type=\"text\" id=\"itemCost\" /><br>"
                    + "<label>Tax Rate :</label><input type=\"text\" id=\"taxRate\" />%<br>"
                    + "<input type=\"submit\" value=\"Calculate\" /></div></form>" + "<br><br><br>"
                    + "<form ACTION=\"../cgi-bin/sales_tax.class\" METHOD=POST>" + "<div id=\"taxCalc\">"
                    + "<label>POST Item Cost: </label>" + "<input type=\"text\" id=\"itemCost\" name=\"cost\" /><br>"

                    + "<label>POST Tax Rate :</label>"

                    + "<input type=\"text\" id=\"taxRate\" name=\"rate\" />%<br>"

                    + "<input type=\"submit\" value=\"Calculate\" /></div></form>" + "<div>" + result + "</div>"
                    + "</body></html>");
        }
    }
}
