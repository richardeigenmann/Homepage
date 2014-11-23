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
                getBooks.query().then(function (response) {
                    self.books = response.data;
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


