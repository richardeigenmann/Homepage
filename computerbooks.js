angular.module('bookApp', ['angular.filter'])
        .factory('getBooks', ['$http', function ($http) {
                return {
                    query: function () {
                        return $http.get('computerbooks.json')
                    },
                };
            }])
        .controller('BookController', ['getBooks', function (getBooks) {
                var self = this;
                self.currentYear = new Date().getFullYear();
                self.BaseYear = 2050;
                getBooks.query().then(function (response) {
                    var data = response.data;
                    // Using a hack here because Angular always sorts keys alphabetically. So
                    // I give it a key to sort on which transforms into the desired key
                    // I.e. The Year 1986 translates to 2050-1986 = 84 which translates back 
                    // as 2050 - 84 => 1986.
                    data.map( function (item) { item.ReverseYear = self.BaseYear - item.ReadYear; })
                    self.books = data;
                });
            }])
        .directive('bookWidget', [function () {
                return {
                    templateUrl: 'bookWidget.html',
                    restrict: 'AE',
                    scope: {
                        bookData: '=',
                    }
                };
            }
        ]);


